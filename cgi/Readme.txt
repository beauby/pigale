This is a simple example showing how to create a Pigale Web Server using a cgi.
The output program is called pigale.cgi and is placed by default in /var/www/cgi-bin.
pigale.cgi is a non graphical program staticlly linked with QtCore and QtNetwork (and do not use the Qt3support).
pigale.cgi connects to pigale which must be started as a server.
The temporary png files are created in /var/www/html/images, hence Apache must have write access to that repertory.

You need to have build the static libraries of Qt, otherwise edit pigale.pro
