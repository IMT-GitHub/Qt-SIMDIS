DCS demo applications:

For Windows, use Q to quit programs, otherwise use Ctl C

sclient - A simple client program implemented from the DCS API
sserver - A simple server program implemented from the raw DCS API
scserver - A simple server program implemented from the DCSServer extension 
           class API.  The extension class makes using the DCS API much 
           simpler, but also introduces implementation limitations that
           require the DCS server to work in a single-threaded/serial 
           processing mode.  This type of server may not handle multiple 
           connections very well.  A concurrent/multi-threaded server can 
           be implemented from the raw DCS API for handling multiple 
           simultaneous connections.  
