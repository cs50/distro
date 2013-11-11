/**
 * service.js
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Rob Bowden
 * rob@cs.harvard.edu
 *
 * Implements a shuttle service.
 */

// default height
var HEIGHT = 0.8;

// default latitude
var LATITUDE = 42.3745615030193;

// default longitude
var LONGITUDE = -71.11803936751632;

// default heading
var HEADING = 1.757197490907891;

// default number of seats
var SEATS = 10;

// default velocity
var VELOCITY = 50;

// global reference to shuttle's marker on 2D map
var bus = null;

// global reference to 3D Earth
var earth = null;

// global reference to geocoding service
var geocoder = null;

// the Konami code: up, up, down, down, left, right, left, right, b, a
var konami_code = [38, 38, 40, 40, 37, 39, 37, 39, 66, 65];

var konami_entered = false;

var konami_index = 0;

// global reference to 2D map
var map = null;

// global array of passengers
var passengers = [];

// global reference to shuttle
var shuttle = null;

// load version 1 of the Google Earth API
google.load("earth", "1");

// load version 3 of the Google Maps API
google.load("maps", "3", {other_params: "sensor=false"});

// once the window has loaded
$(window).load(function() {

    // listen for keydown anywhere in body
    $(document.body).keydown(function(event) {
        return keystroke(event, true);
    });

    // listen for keyup anywhere in body
    $(document.body).keyup(function(event) {
        return keystroke(event, false);
    });

    // listen for click on Drop Off button
    $("#dropoff").click(function(event) {
        dropoff();
    });

    // listen for click on Pick Up button
    $("#pickup").click(function(event) {
        pickup();
    });

    // listen for click on Teleport button
    $("#teleport-btn").click(function(event) {
        var address = $("#address").val();
        teleport(address);
    });

    // prevent the shuttle from intercepting text entered into the input box
    $("#address").keydown(function(event) {
        event.stopPropagation();
    });

    $("#address").keyup(function(event) {
        event.stopPropagation();
    });

    // load application
    load();
});

// unload application
$(window).unload(function() {
    unload();
});

/**
 * Announces message.
 */
function announce(message)
{
    // update DOM
    if (typeof(message) != "undefined")
    {
        $("#announcements").html(message);
    }
    else
    {
        $("#announcements").html("no announcements at this time");
    }
}

/**
 * Renders seating chart.
 */
function chart()
{
    var html = "<ol start='0'>";
    for (var i = 0; i < shuttle.seats.length; i++)
    {
        if (shuttle.seats[i] == null)
        {
            html += "<li>Empty Seat</li>";
        }
        else
        {
            html += "<li>" + shuttle.seats[i].name + " for " + shuttle.seats[i].house + "</li>";
        }
    }
    html += "</ol>";
    $("#chart").html(html);
}

/**
 * Drops up passengers if their stop is nearby.
 */
function dropoff()
{
    // prepare to count dropped-off passengers
    var dropped = 0;

    // drop off passengers
    for (var i = 0; i < shuttle.seats.length; i++)
    {
        // check if passenger's house is nearby
        if (shuttle.seats[i] != null)
        {
            var lat = HOUSES[shuttle.seats[i].house].lat;
            var lng = HOUSES[shuttle.seats[i].house].lng;
            if (shuttle.distance(lat, lng) <= 30.0)
            {
                shuttle.seats[i] = null;
                dropped++;
            }
        }
    }

    // announce if nobody got off
    if (dropped == 0)
    {
        announce("nobody wants to be dropped off here!");
    }

    // re-render seating chart
    chart();
}

/**
 * Called if Google Earth fails to load.
 */
function failureCB(errorCode)
{
    // report error unless plugin simply isn't installed
    if (errorCode != ERR_CREATE_PLUGIN)
    {
        alert(errorCode);
    }
}

/**
 * Handler for Earth's frameend event.
 */
function frameend()
{
    shuttle.update();
}

/**
 * Called once Google Earth has loaded.
 */
function initCB(instance)
{
    // retain reference to GEPlugin instance
    earth = instance;

    // specify the speed at which the camera moves
    earth.getOptions().setFlyToSpeed(100);

    // show buildings
    earth.getLayerRoot().enableLayerById(earth.LAYER_BUILDINGS, true);

    // disable terrain (so that Earth is flat)
    earth.getLayerRoot().enableLayerById(earth.LAYER_TERRAIN, false);

    // prevent mouse navigation in the plugin
    earth.getOptions().setMouseNavigationEnabled(false);

    // instantiate shuttle
    shuttle = new Shuttle({
        heading: HEADING,
        height: HEIGHT,
        latitude: LATITUDE,
        longitude: LONGITUDE,
        planet: earth,
        seats: SEATS,
        velocity: VELOCITY
    });

    // synchronize camera with Earth
    google.earth.addEventListener(earth, "frameend", frameend);

    // synchronize map with Earth
    google.earth.addEventListener(earth.getView(), "viewchange", viewchange);

    // update shuttle's camera
    shuttle.updateCamera();

    // show Earth
    earth.getWindow().setVisibility(true);

    // render seating chart
    chart();

    // populate Earth with passengers and houses
    populate();

    // initialize geocoding service
    geocoder = new google.maps.Geocoder();
}

/**
 * Handles keystrokes.
 */
function keystroke(event, state)
{
    // ensure we have event
    if (!event)
    {
        event = window.event;
    }

    if (!konami_entered && !state)
    {
        if (event.keyCode == konami_code[konami_index])
        {
            konami_index++;
            if (konami_index == konami_code.length)
            {
                konami_entered = true;
            }
        }
        else
        {
            konami_index = 0;
        }
    }

    // left arrow
    if (event.keyCode == 37)
    {
        shuttle.states.turningLeftward = state;
        return false;
    }

    // up arrow
    else if (event.keyCode == 38)
    {
        shuttle.states.tiltingUpward = state;
        return false;
    }

    // right arrow
    else if (event.keyCode == 39)
    {
        shuttle.states.turningRightward = state;
        return false;
    }

    // down arrow
    else if (event.keyCode == 40)
    {
        shuttle.states.tiltingDownward = state;
        return false;
    }

    // A, a
    else if (event.keyCode == 65 || event.keyCode == 97)
    {
        shuttle.states.slidingLeftward = state;
        return false;
    }

    // D, d
    else if (event.keyCode == 68 || event.keyCode == 100)
    {
        shuttle.states.slidingRightward = state;
        return false;
    }

    // S, s
    else if (event.keyCode == 83 || event.keyCode == 115)
    {
        shuttle.states.movingBackward = state;
        return false;
    }

    // W, w
    else if (event.keyCode == 87 || event.keyCode == 119)
    {
        shuttle.states.movingForward = state;
        return false;
    }

    // X, x (speed up)
    else if (event.keyCode == 88 || event.keyCode == 120)
    {
        if (shuttle.velocity < 150)
        {
            shuttle.velocity += 5;
        }
        return false;
    }

    // Z, z (slow down)
    else if (event.keyCode == 90  || event.keyCode == 122)
    {
        if (shuttle.velocity > 5)
        {
            shuttle.velocity -= 5;
        }
        return false;
    }

    else if (konami_entered)
    {
        // C, c (fly down)
        if (event.keyCode == 67 || event.keyCode == 99)
        {
            shuttle.states.flyingDownward = state;
            return false;
        }

        // E, e (fly up)
        if (event.keyCode == 69 || event.keyCode == 101)
        {
            shuttle.states.flyingUpward = state;
            return false;
        }
    }

    return true;
}

/**
 * Loads application.
 */
function load()
{
    // embed 2D map in DOM
    var latlng = new google.maps.LatLng(LATITUDE, LONGITUDE);
    map = new google.maps.Map($("#map").get(0), {
        center: latlng,
        disableDefaultUI: true,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        scrollwheel: false,
        zoom: 17,
        zoomControl: true
    });

    // prepare shuttle's icon for map
    bus = new google.maps.Marker({
        icon: "https://maps.gstatic.com/intl/en_us/mapfiles/ms/micons/bus.png",
        map: map,
        title: "you are here"
    });

    // embed 3D Earth in DOM
    google.earth.createInstance("earth", initCB, failureCB);
}

/**
 * Picks up nearby passengers.
 */
function pickup()
{
    // prepare to remove placemarks
    var features = earth.getFeatures();

    // count passengers picked up
    var picked = 0;

    // iterate over passengers
    for (var i = 0; i < passengers.length; i++)
    {
        // check if passenger is waiting for pickup
        if (!passengers[i].placemark)
        {
            continue;
        }

        // check if passenger isn't in one of the houses
        if (!HOUSES[passengers[i].house])
        {
            continue;
        }

        // get passenger's position
        var lat = passengers[i].placemark.getGeometry().getLatitude();
        var lng = passengers[i].placemark.getGeometry().getLongitude();

        // check distance between shuttle and passenger
        if (shuttle.distance(lat, lng) <= 15.0)
        {
            // try to seat passenger
            var seated = false;
            for (var j = 0; j < shuttle.seats.length; j++)
            {
                // check for empty seat
                if (shuttle.seats[j] == null)
                {
                    // seat passenger
                    shuttle.seats[j] = {
                        name: passengers[i].name,
                        house: passengers[i].house
                    };

                    // remember pick-up
                    picked++;
                    seated = true;

                    // remove placemark from Earth
                    features.removeChild(passengers[i].placemark);
                    passengers[i].placemark = null;

                    // remove marker from map
                    passengers[i].marker.setMap(null);
                    passengers[i].marker = null;

                    // seated!
                    break;
                }
            }
            if (!seated)
            {
                announce("out of seats!");
                chart();
                return;
            }
        }
    }

    // announce if nobody's nearby
    if (picked == 0)
    {
        announce("nobody wants to get picked up here!");
    }

    // re-render seating chart
    chart();
}

/**
 * Populates Earth with passengers and houses.
 */
function populate()
{
    // mark houses
    for (var house in HOUSES)
    {
        // plant house on map
        new google.maps.Marker({
            icon: "https://google-maps-icons.googlecode.com/files/home.png",
            map: map,
            position: new google.maps.LatLng(HOUSES[house].lat, HOUSES[house].lng),
            title: house
        });
    }

    // get current URL, sans any filename
    var url = window.location.href.substring(0, (window.location.href.lastIndexOf("/")) + 1);

    // scatter passengers
    for (var i = 0; i < PASSENGERS.length; i++)
    {
        // pick a random building
        var building = BUILDINGS[Math.floor(Math.random() * BUILDINGS.length)];

        // prepare placemark
        var placemark = earth.createPlacemark("");
        placemark.setName(PASSENGERS[i].name + " to " + PASSENGERS[i].house);

        // prepare icon
        var icon = earth.createIcon("");
        icon.setHref(url + "/img/" + PASSENGERS[i].username + ".jpg");

        // prepare style
        var style = earth.createStyle("");
        style.getIconStyle().setIcon(icon);
        style.getIconStyle().setScale(4.0);

        // prepare stylemap
        var styleMap = earth.createStyleMap("");
        styleMap.setNormalStyle(style);
        styleMap.setHighlightStyle(style);

        // associate stylemap with placemark
        placemark.setStyleSelector(styleMap);

        // prepare point
        var point = earth.createPoint("");
        point.setAltitudeMode(earth.ALTITUDE_RELATIVE_TO_GROUND);
        point.setLatitude(building.lat);
        point.setLongitude(building.lng);
        point.setAltitude(0.0);

        // associate placemark with point
        placemark.setGeometry(point);

        // add placemark to Earth
        earth.getFeatures().appendChild(placemark);

        // add marker to map
        var marker = new google.maps.Marker({
            icon: "https://maps.gstatic.com/intl/en_us/mapfiles/ms/micons/man.png",
            map: map,
            position: new google.maps.LatLng(building.lat, building.lng),
            title: PASSENGERS[i].name + " at " + building.name
        });

        // remember passenger
        passengers.push({
            marker: marker,
            house: PASSENGERS[i].house,
            name: PASSENGERS[i].name,
            placemark: placemark
        });
    }
}

/**
 * Teleport to given address.
 */
function teleport(address)
{
    // geocode address
    geocoder.geocode({ address: address }, function(result, status) {
        if (status != google.maps.GeocoderStatus.OK)
        {
            alert("No can do!");
        }
        else
        {
            // update map
            map.setCenter(result[0].geometry.location);
            bus.setPosition(result[0].geometry.location);

            // update shuttle metadata
            shuttle.position.latitude = result[0].geometry.location.mb;
            shuttle.position.longitude = result[0].geometry.location.nb;
            shuttle.localAnchorCartesian = V3.latLonAltToCartesian([
                shuttle.position.latitude,
                shuttle.position.longitude,
                shuttle.position.altitude
            ]);

            // refresh camera
            shuttle.updateCamera();
        }
    });
}

/**
 * Handler for Earth's viewchange event.
 */
function viewchange()
{
    // keep map centered on shuttle's marker
    var latlng = new google.maps.LatLng(shuttle.position.latitude, shuttle.position.longitude);
    map.setCenter(latlng);
    bus.setPosition(latlng);

    // clear any announcements
    announce(false);
}

/**
 * Unloads Earth.
 */
function unload()
{
    google.earth.removeEventListener(earth.getView(), "viewchange", viewchange);
    google.earth.removeEventListener(earth, "frameend", frameend);
}
