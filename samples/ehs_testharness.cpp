/* $Id$
 *
 * EHS is a library for embedding HTTP(S) support into a C++ application
 *
 * Copyright (C) 2004 Zachary J. Hansen
 *
 * Code cleanup, new features and bugfixes: Copyright (C) 2010 Fritz Elfert
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1 as published by the Free Software Foundation;
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *    This can be found in the 'COPYING' file.
 *
 */

#include <ehs.h>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include "common.h"

using namespace std;

// subclass of EHS that defines a custom HTTP response.
class TestHarness : public EHS
{
    // generates a page for each http request
    ResponseCode HandleRequest(HttpRequest *request, HttpResponse *response)
    {
        ostringstream oss;
        oss
            << "<html><head><title>TestHarness</title></head><body><table><tr>"
            << "<tr><td>request-method:</td><td>" << request->Method() << "</td></tr>" << endl
            << "<tr><td>uri:</td><td>" << request->Uri() << "</td></tr>" << endl
            << "<tr><td>http-version:</td><td>" << request->HttpVersion() << "</td></tr>" << endl
            << "<tr><td>body-length:</td><td>" << request->Body().length() << "</td></tr>" << endl
            << "<tr><td>request-headers #:</td><td>" << request->Headers().size() << "</td></tr>" << endl
            << "<tr><td>formvalue-maps #:</td><td>" << request->FormValues().size() << "</td></tr>" << endl
            << "<tr><td>client-address:</td><td>" << request->RemoteAddress() << "</td></tr>" << endl
            << "<tr><td>client-port:</td><td>" << request->RemotePort() << "</td></tr>" << endl;

        for (StringMap::iterator i = request->Headers().begin();
                i != request->Headers().end(); ++i) {
            oss << "<tr><td>Request Header:</td><td>"
                << i->first << " => " << i->second << "</td></tr>" << endl;
        }

        for ( CookieMap::iterator i = request->Cookies().begin ( );
                i != request->Cookies().end ( ); ++i ) {
            oss << "<tr><td>Cookie:</td><td>"
                << i->first << " => " << i->second << "</td></tr>" << endl;
        }
        oss << "</table></body></html>" << endl;

        response->SetBody ( oss.str().c_str(), oss.str().length() );
        return HTTPRESPONSECODE_200_OK;
    }
};

// basic main that creates a threaded EHS object and then
//   sleeps forever and lets the EHS thread do its job.
int main (int argc, char **argv)
{
    cout << getEHSconfig() << endl;
    if (argc != 2) {
        cerr << "Usage: " << basename(argv[0]) << " [port]" << endl;
        exit (0);
    }
    cerr << "binding to " << atoi(argv[1]) << endl;
    TestHarness srv;

    EHSServerParameters oSP;
    oSP["port"] = argv[1];
    oSP["mode"] = "threadpool";

    try {
        srv.StartServer(oSP);
        kbdio kbd;
        cout << "Press q to terminate ..." << endl;
        while (!(srv.ShouldTerminate() || kbd.qpressed())) {
            usleep(300000);
        }
        srv.StopServer();
    } catch (exception &e) {
        cerr << "ERROR: " << e.what() << endl;
    }

    return 0;
}
