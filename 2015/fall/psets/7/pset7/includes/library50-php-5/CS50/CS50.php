<?php

    /**
     * CS50 Library 5
     * https://manual.cs50.net/library/
     *
     * @author David J. Malan <malan@harvard.edu>
     * @link https://manual.cs50.net/library/
     * @package CS50
     * @version 5
     * 
     * Copyright (c) 2015
     * David J. Malan <malan@harvard.edu>
     * All Rights Reserved
     *
     * BSD 3-Clause License
     * http://www.opensource.org/licenses/BSD-3-Clause
     * 
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions are
     * met:
     *
     * * Redistributions of source code must retain the above copyright notice,
     *   this list of conditions and the following disclaimer.
     * * Redistributions in binary form must reproduce the above copyright
     *   notice, this list of conditions and the following disclaimer in the
     *   documentation and/or other materials provided with the distribution.
     * * Neither the name of CS50 nor the names of its contributors may be used
     *   to endorse or promote products derived from this software without
     *   specific prior written permission.
     *
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
     * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
     * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
     * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
     * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
     * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
     * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
     * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
     * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     */

    // require extensions for Janrain's libary
    if (!extension_loaded("bcmath") && !extension_loaded("gmp"))
    	trigger_error("CS50 Library requires bcmath or gmp extension module", E_USER_ERROR);
    if (!extension_loaded("dom") && !extension_loaded("domxml"))
    	trigger_error("CS50 Library requires dom or domxml extension module", E_USER_ERROR);
    if (!extension_loaded("openssl"))
    	trigger_error("CS50 Library requires openssl extension module", E_USER_ERROR);

    // ensure Janrain's library doesn't fail on Windows
    if (strtoupper(substr(PHP_OS, 0, 3)) === "WIN")
    {
        // because /dev/urandom doesn't exist
        define("Auth_OpenID_RAND_SOURCE", null);
 
        // because php_curl.dll doesn't come with ca-bundle.crt
        define("Auth_Yadis_CURL_OVERRIDE", null);
    }

    // CS50
    class CS50
    {
        /**
         *
         */
        private static $config;

        /**
         *
         */
        public static function init($path)
        {
            //
            if (isset(self::$config))
            {
                trigger_error("CS50 Library is already initialized", E_USER_ERROR);
            }

            // ensure configuration file exists
            if (!is_file($path))
            {
                trigger_error("Could not find $path", E_USER_ERROR);
            }

            // read contents of configuration file
            $contents = file_get_contents($path);
            if ($contents === false)
            {
                trigger_error("Could not read $path", E_USER_ERROR);
            }

            // decode contents of configuration file
            $config = json_decode($contents, true);
            if (is_null($config))
            {
                trigger_error("Could not decode $path", E_USER_ERROR);
            }

            // store configuration
            self::$config = $config;
        }

        /**
         *
         */
        public static function dump(/* args */)
        {
            print("<!DOCTYPE html>\n\n");
            print("<html><head><title>dump</title></head><body><pre>");
            foreach (func_get_args() as $arg)
            {
                var_dump($arg);
            }
            print("</pre></body></html>");
            exit;
        }

        /**
         * Returns URL to which user can be directed for 
         * authentication via CS50 ID.
         *
         * @param trust_root  URL that CS50 ID should prompt user to trust
         * @param return_to   URL to which CS50 ID should return user
         * @param fields      Simple Registration fields to request from CS50 ID
         * @param attributes  Attribute Exchange attributes to request from CS50 ID
         *
         * @return URL for CS50 ID
         */
        public static function getLoginUrl($trust_root, $return_to, $fields = ["email", "fullname"], $attributes = [])
        {
            // TODO: comment, add check for config vars
            if (isset(self::$config))
            {
                trigger_error("CS50 Library is not initialized", E_USER_ERROR);
            }

            // ignore Janrain's use of deprecated functions
            $error_reporting = error_reporting();
            error_reporting($error_reporting & ~E_DEPRECATED);

            // load Janrain's libary
            set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__) . DIRECTORY_SEPARATOR . "share" . DIRECTORY_SEPARATOR . "php-openid-2.3.0");
            require_once("Auth/OpenID/AX.php");
            require_once("Auth/OpenID/Consumer.php");
            require_once("Auth/OpenID/FileStore.php");
            require_once("Auth/OpenID/SReg.php");

            // ensure $_SESSION exists for Yadis
            @session_start();

            // prepare filesystem-based store
            $path = sys_get_temp_dir() . DIRECTORY_SEPARATOR . md5($return_to);
            @mkdir($path, 0700);
            if (!is_dir($path))
                trigger_error("Could not create $path", E_USER_ERROR);
            if (!is_readable($path))
                trigger_error("Could not read from $path", E_USER_ERROR);
            if (!is_writable($path))
                trigger_error("Could not write to $path", E_USER_ERROR);
            $store = new Auth_OpenID_FileStore($path);

            // prepare request
            $consumer = new Auth_OpenID_Consumer($store);
            $auth_request = $consumer->begin("https://id.cs50.net/");

            // request Simple Registration fields
            if (is_array($fields) && count($fields) > 0)
            {
                $sreg_request = Auth_OpenID_SRegRequest::build(null, $fields);
                $auth_request->addExtension($sreg_request);
            }

            // request Attribute Exchange attributes
            if (is_array($attributes) && count($attributes) > 0)
            {
                $ax_request = new Auth_OpenID_AX_FetchRequest();
                foreach ($attributes as $attribute)
                    $ax_request->add(Auth_OpenID_AX_AttrInfo::make($attribute, 1, false));
                $auth_request->addExtension($ax_request);
            }

            // generate URL for redirection
            $redirect_url = $auth_request->redirectURL($trust_root, $return_to);

            // restore error_reporting
            error_reporting($error_reporting);

            // return URL unless error
            if (Auth_OpenID::isFailure($redirect_url))
            {
                trigger_error($redirect_url->message);
                return false;
            }
            else
                return $redirect_url;
        }
 
        /**
         * Iff user was authenticated (at URL returned by getLoginUrl),
         * returns associative array that WILL contain user's Harvard email
         * address (mail) and that MAY contain user's name (displayName).
         *
         * @param return_to  URL to which CS50 ID returned user
         *
         * @return user as associative array
         */
        public static function getUser($return_to)
        {
            // TODO: comment, add check for config vars
            if (isset(self::$config))
            {
                trigger_error("CS50 Library is not initialized", E_USER_ERROR);
            }

            // ignore Janrain's use of deprecated functions
            $error_reporting = error_reporting();
            error_reporting($error_reporting & ~E_DEPRECATED);

            // load Janrain's libary
            set_include_path(get_include_path() . PATH_SEPARATOR . dirname(__FILE__) . DIRECTORY_SEPARATOR . "share" . DIRECTORY_SEPARATOR . "php-openid-2.3.0");
            require_once("Auth/OpenID/AX.php");
            require_once("Auth/OpenID/Consumer.php");
            require_once("Auth/OpenID/FileStore.php");
            require_once("Auth/OpenID/SReg.php");

            // ensure $_SESSION exists for Yadis
            @session_start();

            // prepare filesystem-based store
            $path = sys_get_temp_dir() . DIRECTORY_SEPARATOR . md5($return_to);
            @mkdir($path, 0700);
            if (!is_dir($path))
                trigger_error("Could not create $path", E_USER_ERROR);
            if (!is_readable($path))
                trigger_error("Could not read from $path", E_USER_ERROR);
            if (!is_writable($path))
                trigger_error("Could not write to $path", E_USER_ERROR);
            $store = new Auth_OpenID_FileStore($path);

            // get response
            $consumer = new Auth_OpenID_Consumer($store);
            $response = $consumer->complete($return_to);
            if ($response->status == Auth_OpenID_SUCCESS)
            {
                // get user's identity
                $user = ["identity" => $response->identity_url];

                // get Simple Registration fields, if any
                if ($sreg_resp = Auth_OpenID_SRegResponse::fromSuccessResponse($response))
                    $user = array_merge($user, $sreg_resp->contents());

                // get Attribute Exchange attributes, if any
                if ($ax_resp = Auth_OpenID_AX_FetchResponse::fromSuccessResponse($response))
                    $user = array_merge($user, $ax_resp->data);
            }

            // restore error_reporting
            error_reporting($error_reporting);

            // return user unless error
            return (isset($user)) ? $user : false;
        }

        /**
         * Executes SQL statement, possibly with parameters, returning
         * an array of all rows in result set or false on (non-fatal) error.
         */
        public static function query(/* $sql [, ... ] */)
        {
            if (!isset(self::$config["database"]))
            {
                trigger_error("Missing value for database", E_USER_ERROR);
            }
            if (!isset(self::$config["database"]["name"]))
            {
                trigger_error("Missing value for database.name", E_USER_ERROR);
            }
            if (!isset(self::$config["database"]["password"]))
            {
                trigger_error("Missing value for database.password", E_USER_ERROR);
            }
            if (!isset(self::$config["database"]["server"]))
            {
                trigger_error("Missing value for database.server", E_USER_ERROR);
            }
            if (!isset(self::$config["database"]["username"]))
            {
                trigger_error("Missing value for database.username", E_USER_ERROR);
            }

            // SQL statement
            $sql = func_get_arg(0);

            // parameters, if any
            $parameters = array_slice(func_get_args(), 1);

            // try to connect to database
            static $handle;
            if (!isset($handle))
            {
                try
                {
                    // connect to database
                    $handle = new PDO("mysql:dbname=" . DATABASE . ";host=" . SERVER, USERNAME, PASSWORD);
                }
                catch (Exception $e)
                {
                    // trigger (big, orange) error
                    trigger_error($e->getMessage(), E_USER_ERROR);
                }
            }

            // ensure number of placeholders matches number of values
            $placeholders = substr_count($sql, "?");
            if ($placeholders < count($parameters))
            {
                trigger_error("Too few placeholders in query", E_USER_ERROR);
            }
            else if ($placeholders > count($parameters))
            {
                trigger_error("Too many placeholders in query", E_USER_ERROR);
            }

            // replace placeholders with quoted, escaped strings
            $pattern = [];
            $replacement = [];
            for ($i = 0; $i < $placeholders; $i++)
            {
                array_push($pattern, "/[?]/");
                array_push($replacement, $handle->quote($parameters[$i]));
            }
            $query = preg_replace($pattern, $replacement, $sql);

            // execute query
            $statement = $handle->query($query);
            if ($statement === false)
            {
                trigger_error($handle->errorInfo()[2], E_USER_ERROR);
            }

            // if query was SELECT
            // http://stackoverflow.com/a/19794473/5156190
            if ($statement->columnCount() > 0)
            {
                // return result set's rows
                return $statement->fetchAll(PDO::FETCH_ASSOC);
            }

            // if query was DELETE, INSERT, or UPDATE
            else
            {
                // return number of columns affected
                return $statement->rowCount();
            }
        }

        /**
         * Redirects user to specified location, which can be
         * a URL or a relative path on the local host.
         *
         * http://stackoverflow.com/a/25643550/5156190
         *
         * Because this function outputs an HTTP header, it
         * must be called before caller outputs any HTML.
         */
        public static function redirect($location)
        {
            if (headers_sent($file, $line))
            {
                trigger_error("HTTP headers already sent at {$file}:{$line}", E_USER_ERROR);
            }
            exit;
        }

        /**
         * Renders template, passing in values.
         */
        public static function render($template, $values = [])
        {
            // if template exists, render it
            if (file_exists("../templates/$template"))
            {
                // extract variables into local scope
                extract($values);

                // render header
                require("../templates/header.php");

                // render template
                require("../templates/$template");

                // render footer
                require("../templates/footer.php");
            }

            // else err
            else
            {
                trigger_error("Invalid template: $template", E_USER_ERROR);
            }
        }
    }

?>
