/**********************************************************************************/
/** __          __  _                                                            **/
/** \ \        / / | |                                by:                        **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __                                   **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / /        Eromon Agbomeirele         **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /         Casimir Lundberg           **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/          Tim Campbell               **/
/**                                                                              **/
/**                                                                              **/
/**                                W E B S E R V                                 **/
/**********************************************************************************/

#include "CGIparsing.hpp"

CGIparsing::CGIparsing(HttpRequest& request) : _pathInfo(request.path), _contentType(request.headers.at("Accept")), _contentLength(request.headers.at("Content-Length")) {
	switch (request.method)
	{
	case 1:
		_method = "GET";
		break;
	case 2:
		_method = "POST";
		break;
	default:
		/*error maybe if none of these found?*/
		break;
	}
	
}