/**
 * scripts.js
 *
 * Computer Science 50
 * Problem Set 8
 *
 * Global JavaScript.
 */

// Google Map
var map;

// markers for map
var markers = [];

// info window
var info = new google.maps.InfoWindow();

// execute when the DOM is fully loaded
$(function() {

    // styles for map
    // https://developers.google.com/maps/documentation/javascript/styling
    var styles = [

        // hide Google's labels
        {
            featureType: "all",
            elementType: "labels",
            stylers: [
                { visibility: "off" }
            ]
        },

        // hide roads
        {
            featureType: "road",
            elementType: "geometry",
            stylers: [
                { visibility: "off" }
            ]
        }

    ];

    // options for map
    // https://developers.google.com/maps/documentation/javascript/maptypes
    var options = {
        center: { lat: 42.375892, lng: -71.114792 }, // Cambridge, Massachusetts
        disableDefaultUI: true,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
        maxZoom: 14,
        panControl: true,
        styles: styles,
        zoom: 13,
        zoomControl: true
    };

    // get DOM node in which map will be instantiated
    var canvas = $("#map-canvas").get(0);

    // instantiate map
    map = new google.maps.Map(canvas, options);

    // configure UI once Google Map is idle (i.e., loaded)
    google.maps.event.addListenerOnce(map, "idle", configure);

});

/**
 * Adds marker for place to map.
 */
function addMarker(place)
{
    // instantiate marker
    var marker = new MarkerWithLabel({
        icon: "http://maps.google.com/mapfiles/kml/pal2/icon31.png", // http://www.lass.it/Web/viewer.aspx?id=4
        labelAnchor: new google.maps.Point(22, 0),
        labelClass: "label",
        labelContent: place.place_name + ", " + place.admin_name1,
        position: new google.maps.LatLng(place.latitude, place.longitude),
        map: map
    });

    // listen for clicks on marker
    google.maps.event.addListener(marker, "click", function() {

        // show ajax indicator
        showInfo(marker);

        // get articles for place (asynchronously)
        $.getJSON("articles.php", { geo: place.postal_code }, function(data) {

            // if no data, no news
            if (data.length === 0)
            {
                showInfo(marker, "Slow news day!");
            }

            // else build unordered list of links to articles
            else
            {
                // instantiate unordered list
                var ul = $("<ul>");

                // iterate over articles
                for (var i = 0; i < data.length; i++)
                {
                    // instantiate, configure anchor
                    var a = $("<a>").text(data[i].title);
                    a.attr("href", data[i].link);
                    a.attr("target", "_blank");

                    // instantiate list item
                    var li = $("<li>");

                    // append anchor to list item
                    li.append(a);

                    // append list item to unordered list
                    ul.append(li);
                }

                // show info window at marker with content
                showInfo(marker, ul.get(0).outerHTML);
            }
        });
    });

    // remember marker (so we can remove it later)
    markers.push(marker);
}

/**
 * Configures application.
 */
function configure()
{
    // update UI after map has been dragged
    google.maps.event.addListener(map, "dragend", function(eventObject) {
        update();
    });

    // update UI after zoom level changes
    google.maps.event.addListener(map, "zoom_changed", function(eventObject) {
        update();
    });

    // remove markers whilst dragging
    google.maps.event.addListener(map, "dragstart", function(eventObject) {
        removeMarkers();
    });

    // configure typeahead
    // https://github.com/twitter/typeahead.js/blob/master/doc/jquery_typeahead.md
    $("#q").typeahead({
        autoselect: true,
        highlight: true,
        minLength: 2
    },
    {
        displayKey: "place_name",
        source: search,
        templates: {
            empty: "no places found yet",
            suggestion: Handlebars.compile("<p>" +
                "<span class='place_name'>{{place_name}}</span> <span class='postal_code'>{{postal_code}}</span>" +
                "</p>")
        }
    });

    // re-center map and update UI after place is selected from drop-down
    $("#q").on("typeahead:selected", function(eventObject, suggestion, name) {
        map.setCenter({lat: suggestion.latitude, lng: suggestion.longitude});
        update();
    });

    // hide info window when text box has focus
    $("#q").focus(function(eventData) {
        hideInfo();
    });

    // re-enable ctrl- and right-clicking (and thus Inspect Element) on Google Map
    // https://chrome.google.com/webstore/detail/allow-right-click/hompjdfbfmmmgflfjdlnkohcplmboaeo?hl=en
    document.addEventListener("contextmenu", function(event) {
        event.returnValue = true; 
        event.stopPropagation && event.stopPropagation(); 
        event.cancelBubble && event.cancelBubble();
    }, true);

    // update UI
    update();

    // give focus to text box
    $("#q").focus();
}

/**
 * Hides info window.
 */
function hideInfo()
{
    info.close();
}

/**
 * Removes markers from map.
 */
function removeMarkers()
{
    for (var i = 0; i < markers.length; i++)
    {
        markers[i].setMap(null);
    }
    markers.length = 0;
}

/**
 * Searches database.
 */
function search(query, cb)
{
    // get places matching query (asynchronously)
    var parameters = {
        geo: query
    };
    $.getJSON("search.php", parameters, function(data) {

        // call callback with data
        cb(data);       
    });
}

/**
 * Shows info window at marker with content.
 */
function showInfo(marker, content)
{
    // start div
    var div = "<div id='info'>";
    if (typeof(content) === "undefined")
    {
        // http://www.ajaxload.info/
        div += "<img alt='loading' src='img/ajax-loader.gif'/>";
    }
    else
    {
        div += content;
    }

    // end div
    div += "</div>";

    // set info window's content
    info.setContent(div);

    // open info window (if not already open)
    info.open(map, marker);
}

/**
 * Updates UI.
 */
function update() 
{
    // get map's bounds
    var bounds = map.getBounds();
    var ne = bounds.getNorthEast();
    var sw = bounds.getSouthWest();

    // get places within bounds (asynchronously)
    var parameters = {
        ne: ne.lat() + "," + ne.lng(),
        q: $("#q").val(),
        sw: sw.lat() + "," + sw.lng()
    };
    $.getJSON("update.php", parameters, function(data) {

        // remove old markers from map
        removeMarkers();

        // add new markers to map
        for (var i = 0; i < data.length; i++)
        {
            addMarker(data[i]);
        }
    });
};
