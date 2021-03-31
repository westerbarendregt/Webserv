#include "RequestHandler.hpp"
#include "Logger.hpp"

void	RequestHandler::initMimeTypes() {

	m_mime_types["aac"] = "audio/aac";
	m_mime_types["abw"] = "application/x-abiword";
	m_mime_types["arc"] = "application/x-freearc";
	m_mime_types["avi"] = "video/x-msvideo";
	m_mime_types["azw"] = "application/vnd.amazon.ebook";
	m_mime_types["bin"] = "application/octet-stream";
	m_mime_types["bmp"] = "image/bmp";
	m_mime_types["bz"] = "application/x-bzip";
	m_mime_types["bz2"] = "application/x-bzip2";
	m_mime_types["csh"] = "application/x-csh";
	m_mime_types["css"] = "text/css";
	m_mime_types["csv"] = "text/csv";
	m_mime_types["doc"] = "application/msword";
	m_mime_types["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	m_mime_types["eot"] = "application/vnd.ms-fontobject";
	m_mime_types["epub"] = "application/epub+zip";
	m_mime_types["gz"] = "application/gzip";
	m_mime_types["gif"] = "image/gif";
	m_mime_types["htm"] = "text/html";
	m_mime_types["html"] = "text/html";
	m_mime_types["ico"] = "image/vnd.microsoft.icon";
	m_mime_types["ics"] = "text/calendar";
	m_mime_types["jar"] = "application/java-archive";
	m_mime_types["jpeg"] = "image/jpeg";
	m_mime_types["jpg"] = "image/jpeg";
	m_mime_types["js"] = "text/javascript";
	m_mime_types["json"] = "application/json";
	m_mime_types["jsonld"] = "application/ld+json";
	m_mime_types["mid"] = "audio/midi";
	m_mime_types["midi"] = "audio/x-midi";
	m_mime_types["mjs"] = "text/javascript";
	m_mime_types["mp3"] = "audio.mpeg";
	m_mime_types["mpeg"] = "video/mpeg";
	m_mime_types["mpkg"] = "application/vnd.apple.installer+xml";
	m_mime_types["odp"] = "application/vnd.oasis.opendocument.presentation";
	m_mime_types["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	m_mime_types["odt"] = "application/vnd.oasis.opendocument.text";
	m_mime_types["oga"] = "audio/ogg";
	m_mime_types["ogv"] = "video/ogg";
	m_mime_types["ogx"] = "application/ogg";
	m_mime_types["opus"] = "audio/opus";
	m_mime_types["otf"] = "font/otf";
	m_mime_types["png"] = "image/png";
	m_mime_types["pdf"] = "application/pdf";
	m_mime_types["php"] = "application/x-httpd-php";
	m_mime_types["ppt"] = "application/vnd.ms-powerpoint";
	m_mime_types["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	m_mime_types["rar"] = "application/vnd.rar";
	m_mime_types["rtf"] = "application/rtf";
	m_mime_types["sh"] = "application/x-sh";
	m_mime_types["svg"] = "image/svg+xml";
	m_mime_types["swf"] = "application/x-shockwave-flash";
	m_mime_types["tar"] = "application/x-tar";
	m_mime_types["tif"] = "image/tiff";
	m_mime_types["tiff"] = "image/tiff";
	m_mime_types["ts"] = "video/mp2t";
	m_mime_types["ttf"] = "font/ttf";
	m_mime_types["txt"] = "text/plain";
	m_mime_types["vsd"] = "application/vnd.visio";
	m_mime_types["wav"] = "audio/wav";
	m_mime_types["weba"] = "audio/webm";
	m_mime_types["webm"] = "video/webm";
	m_mime_types["webp"] = "image/webp";
	m_mime_types["woff"] = "font/woff";
	m_mime_types["woff2"] = "font/woff2";
	m_mime_types["xhtml"] = "application/xhtml+xml";
	m_mime_types["xls"] = "application/vnd.ms-excel";
	m_mime_types["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	m_mime_types["xml"] = "application/xml";
	m_mime_types["xul"] = "application/vnd.mozilla.xul+xml";
	m_mime_types["zip"] = "application/zip";
	m_mime_types["3gp"] = "video/3gpp";
	m_mime_types["3g2"] = "video/3gpp2";
	m_mime_types["7z"] = "application/x-7z-compressed";
}

void	RequestHandler::printMimeTypes() {
	std::map<std::string, std::string>::iterator	it;

	for (it = m_mime_types.begin(); it != m_mime_types.end(); ++it) {
		Logger::Log() << it->first << " => " << m_mime_types[it->first] << std::endl;
	}
}
