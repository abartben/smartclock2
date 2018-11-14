<?php

error_reporting(E_ALL);

/* Allow the script to hang around waiting for connections. */
set_time_limit(0);

$address = '192.168.1.2'; //your ip adress

$port = 6969; //port

// create a streaming socket, of type TCP/IP
$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);

socket_set_option($sock, SOL_SOCKET, SO_REUSEADDR, 1);

socket_bind($sock, $address, $port);

socket_listen($sock);

// create a list of all the clients that will be connected to us..
// add the listening socket to this list
$clients = array($sock);

while (true)
{
    // create a copy, so $clients doesn't get modified by socket_select()
    $read = $clients;
    $write = null;
    $except = null;

    // get a list of all the clients that have data to be read from
    // if there are no clients with data, go to next iteration
    if (socket_select($read, $write, $except, 0) < 1)
        continue;

    // check if there is a client trying to connect
    if (in_array($sock, $read))
    {
        $clients[] = $newsock = socket_accept($sock);

        //socket_write($newsock, "There are ".(count($clients) - 1)." client(s) connected to the server\n");

	socket_write($newsock, "Bonjour bonjour tout va bien l'eau le gaz et l'électricité\n");

        socket_getpeername($newsock, $ip, $port);
        echo "New client connected: {$ip}\n";

        $key = array_search($sock, $read);
        unset($read[$key]);
    }

    // loop through all the clients that have data to read from
    foreach ($read as $read_sock)
    {
        // read until newline or 1024 bytes
        // socket_read while show errors when the client is disconnected, so silence the error messages
        $data = @socket_read($read_sock, 4096, PHP_BINARY_READ);

        // check if the client is disconnected
        if ($data === false)
        {
            // remove client for $clients array
            $key = array_search($read_sock, $clients);
            unset($clients[$key]);
            echo "client disconnected.\n";
            continue;
        }

        $data = trim($data);

        if (!empty($data))
        {
            //echo " send {$data}\n";
            if($data == "1")
            {
              $msg = ""; //create your message here
              socket_write($read_sock,$msg);
            }
            elseif($data == "2")
            {
              $msg = exec("php /var/www/script/reveil2.php");
              socket_write($read_sock,$msg);
            }
            
        }

    } // end of reading foreach
}

// close the listening socket
socket_close($sock);

?>
