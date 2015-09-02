# Linear FastCGI

## Overview

This application enable to connect and forward messages to a msgpack-rpc(TCP) server from IE etc. that is unable to use WebSocket protocol.

### Notice
Many browsers support WebSocket protocol by default recently, in near future, there'll be no need to use this application.  
So we does not update this program unless a big problem occurs.

## Build Instructions
### Required tools and Dependencies
* xNix based WebServer<br>
  this application works with only xNix based OS.
* autotools and libtool<br>
  aclocal, autoheader, automake, autoconf, libtoolize
* lex and yacc<br>
  flex 2.5.35 or later<br>
  bison 2.4.1 or later
* fastcgi C library<br>
  libfcgi 2.4.0 or later
* Perl Regular Expression library<br>
  libpcre
* lighttpd with mod\_websocket(included at this repo)<br>
  use mod\_websocket\_config\_parser lib

## HowToMake
<pre class="fragment">
$ ./bootstrap
$ ./configure --with-fastcgi[=/path/to/libfcgi]
$ make clean all
$ cp app/linear_fcgi /path/to/cgi-bin/linear.fcgi
</pre>

## Additional Informations
* Basic flow ( C is linear.js and S is a linear.fcgi )<br>
linear.fcgi uses Content-type: [application/javascript](http://www.ietf.org/rfc/rfc4329.txt) for JSONP response.<br>
So linear.fcgi does not work well in prior to IE8.
Refer to [this stackoverflow](http://stackoverflow.com/questions/111302/best-content-type-to-serve-jsonp)<br>
<pre class="fragment">
    C                                                             S
    | --- GET                                                     |
    |     /path/to/linear.fcgi?                                   |
    |     c=connect&ch=somechannel&f=callback                 --> |
    | <-- 200 OK                                                  |
    |     body = callback({sid: session-id-created-by-S})     --- |
    | --- POST                                                    |
    |     /path/to/linear.fcgi                                    |
    |     body = {sid: sid-id-created-by-S,                       |
    |             data: base64-encoded-msgpack-rpc}           --> |
    | <-- 204 No Content                                      --- |
    | --- GET                                                     |
    |     /path/to/linear.fcgi?                                   |
    |     c=poll&sid=sid-id-created-by-S&f=callback           --> |
    | <-- 200 OK                                                  |
    |     body = callback({sid: sid-id-created-by-S,              |
    |                      data: base64-encoded-msgpack-rpc}) --- |
    | --- GET                                                     |
    |     /path/to/linear.fcgi?                                   |
    |     c=disconnect&sid=sid-id-created-by-S&f=callback     --> |
    | <-- 200 OK                                                  |
    |     body = callback({sid: session-id-created-by-S})     --- |

</pre>

* Security consideration
<dl>
  <dt>HTTP Response headers</dt><dd>When using linear.fcgi, you should add some HTTP headers.<br>
  1. X-Frame-Options<br>
  2. X-XSS-Protection<br>
  Refer to [List of useful HTTP headers](https://www.owasp.org/index.php/List_of_useful_HTTP_headers)</dd>
  <dt>CSRF</dt><dd>An evil site can't know session id.<br>
  And server-side can use origin limitation by configuring websocket.conf for lighttpd.</dd>
  <dt>XSS</dt><dd>linear.js does not use any eval functions.<br>
  And linear.js does only pass some json messages to applications.</dd>
  <dt>Authentication</dt><dd>Use {Basic, Digest} auth and use SSL(https) instead of http.<br>
</dl>

## License
The MIT License (MIT)  
See LICENSE for details.  

And see some submodule LICENSEs(exist at deps dir).
