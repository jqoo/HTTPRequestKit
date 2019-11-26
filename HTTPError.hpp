//
//  HTTPError.hpp
//  pubkey-pin
//
//  Created by jqoo on 2016/12/21.
//  Copyright © 2016年 OWASP. All rights reserved.
//

#ifndef HTTPError_hpp
#define HTTPError_hpp

#include <string>

class HTTPError {
public:
    typedef enum {
        HTTPError_Code_OK = 0,
        HTTPError_Code_UNSUPPORTED_PROTOCOL,    /* 1 */
        HTTPError_Code_FAILED_INIT,             /* 2 */
        HTTPError_Code_URL_MALFORMAT,           /* 3 */
        HTTPError_Code_NOT_BUILT_IN,            /* 4 - [was obsoleted in August 2007 for
                                        7.17.0, reused in April 2011 for 7.21.5] */
        HTTPError_Code_COULDNT_RESOLVE_PROXY,   /* 5 */
        HTTPError_Code_COULDNT_RESOLVE_HOST,    /* 6 */
        HTTPError_Code_COULDNT_CONNECT,         /* 7 */
        HTTPError_Code_WEIRD_SERVER_REPLY,      /* 8 */
        HTTPError_Code_REMOTE_ACCESS_DENIED,    /* 9 a service was denied by the server
                                        due to lack of access - when login fails
                                        this is not returned. */
        HTTPError_Code_FTP_ACCEPT_FAILED,       /* 10 - [was obsoleted in April 2006 for
                                        7.15.4, reused in Dec 2011 for 7.24.0]*/
        HTTPError_Code_FTP_WEIRD_PASS_REPLY,    /* 11 */
        HTTPError_Code_FTP_ACCEPT_TIMEOUT,      /* 12 - timeout occurred accepting server
                                        [was obsoleted in August 2007 for 7.17.0,
                                        reused in Dec 2011 for 7.24.0]*/
        HTTPError_Code_FTP_WEIRD_PASV_REPLY,    /* 13 */
        HTTPError_Code_FTP_WEIRD_227_FORMAT,    /* 14 */
        HTTPError_Code_FTP_CANT_GET_HOST,       /* 15 */
        HTTPError_Code_HTTP2,                   /* 16 - A problem in the http2 framing layer.
                                        [was obsoleted in August 2007 for 7.17.0,
                                        reused in July 2014 for 7.38.0] */
        HTTPError_Code_FTP_COULDNT_SET_TYPE,    /* 17 */
        HTTPError_Code_PARTIAL_FILE,            /* 18 */
        HTTPError_Code_FTP_COULDNT_RETR_FILE,   /* 19 */
        HTTPError_Code_OBSOLETE20,              /* 20 - NOT USED */
        HTTPError_Code_QUOTE_ERROR,             /* 21 - quote command failure */
        HTTPError_Code_HTTP_RETURNED_ERROR,     /* 22 */
        HTTPError_Code_WRITE_ERROR,             /* 23 */
        HTTPError_Code_OBSOLETE24,              /* 24 - NOT USED */
        HTTPError_Code_UPLOAD_FAILED,           /* 25 - failed upload "command" */
        HTTPError_Code_READ_ERROR,              /* 26 - couldn't open/read from file */
        HTTPError_Code_OUT_OF_MEMORY,           /* 27 */
        /* Note: HTTPError_Code_OUT_OF_MEMORY may sometimes indicate a conversion error
         instead of a memory allocation error if CURL_DOES_CONVERSIONS
         is defined
         */
        HTTPError_Code_OPERATION_TIMEDOUT,      /* 28 - the timeout time was reached */
        HTTPError_Code_OBSOLETE29,              /* 29 - NOT USED */
        HTTPError_Code_FTP_PORT_FAILED,         /* 30 - FTP PORT operation failed */
        HTTPError_Code_FTP_COULDNT_USE_REST,    /* 31 - the REST command failed */
        HTTPError_Code_OBSOLETE32,              /* 32 - NOT USED */
        HTTPError_Code_RANGE_ERROR,             /* 33 - RANGE "command" didn't work */
        HTTPError_Code_HTTP_POST_ERROR,         /* 34 */
        HTTPError_Code_SSL_CONNECT_ERROR,       /* 35 - wrong when connecting with SSL */
        HTTPError_Code_BAD_DOWNLOAD_RESUME,     /* 36 - couldn't resume download */
        HTTPError_Code_FILE_COULDNT_READ_FILE,  /* 37 */
        HTTPError_Code_LDAP_CANNOT_BIND,        /* 38 */
        HTTPError_Code_LDAP_SEARCH_FAILED,      /* 39 */
        HTTPError_Code_OBSOLETE40,              /* 40 - NOT USED */
        HTTPError_Code_FUNCTION_NOT_FOUND,      /* 41 */
        HTTPError_Code_ABORTED_BY_CALLBACK,     /* 42 */
        HTTPError_Code_BAD_FUNCTION_ARGUMENT,   /* 43 */
        HTTPError_Code_OBSOLETE44,              /* 44 - NOT USED */
        HTTPError_Code_INTERFACE_FAILED,        /* 45 - CURLOPT_INTERFACE failed */
        HTTPError_Code_OBSOLETE46,              /* 46 - NOT USED */
        HTTPError_Code_TOO_MANY_REDIRECTS,      /* 47 - catch endless re-direct loops */
        HTTPError_Code_UNKNOWN_OPTION,          /* 48 - User specified an unknown option */
        HTTPError_Code_TELNET_OPTION_SYNTAX,    /* 49 - Malformed telnet option */
        HTTPError_Code_OBSOLETE50,              /* 50 - NOT USED */
        HTTPError_Code_PEER_FAILED_VERIFICATION, /* 51 - peer's certificate or fingerprint
                                         wasn't verified fine */
        HTTPError_Code_GOT_NOTHING,             /* 52 - when this is a specific error */
        HTTPError_Code_SSL_ENGINE_NOTFOUND,     /* 53 - SSL crypto engine not found */
        HTTPError_Code_SSL_ENGINE_SETFAILED,    /* 54 - can not set SSL crypto engine as
                                        default */
        HTTPError_Code_SEND_ERROR,              /* 55 - failed sending network data */
        HTTPError_Code_RECV_ERROR,              /* 56 - failure in receiving network data */
        HTTPError_Code_OBSOLETE57,              /* 57 - NOT IN USE */
        HTTPError_Code_SSL_CERTPROBLEM,         /* 58 - problem with the local certificate */
        HTTPError_Code_SSL_CIPHER,              /* 59 - couldn't use specified cipher */
        HTTPError_Code_SSL_CACERT,              /* 60 - problem with the CA cert (path?) */
        HTTPError_Code_BAD_CONTENT_ENCODING,    /* 61 - Unrecognized/bad encoding */
        HTTPError_Code_LDAP_INVALID_URL,        /* 62 - Invalid LDAP URL */
        HTTPError_Code_FILESIZE_EXCEEDED,       /* 63 - Maximum file size exceeded */
        HTTPError_Code_USE_SSL_FAILED,          /* 64 - Requested FTP SSL level failed */
        HTTPError_Code_SEND_FAIL_REWIND,        /* 65 - Sending the data requires a rewind
                                        that failed */
        HTTPError_Code_SSL_ENGINE_INITFAILED,   /* 66 - failed to initialise ENGINE */
        HTTPError_Code_LOGIN_DENIED,            /* 67 - user, password or similar was not
                                        accepted and we failed to login */
        HTTPError_Code_TFTP_NOTFOUND,           /* 68 - file not found on server */
        HTTPError_Code_TFTP_PERM,               /* 69 - permission problem on server */
        HTTPError_Code_REMOTE_DISK_FULL,        /* 70 - out of disk space on server */
        HTTPError_Code_TFTP_ILLEGAL,            /* 71 - Illegal TFTP operation */
        HTTPError_Code_TFTP_UNKNOWNID,          /* 72 - Unknown transfer ID */
        HTTPError_Code_REMOTE_FILE_EXISTS,      /* 73 - File already exists */
        HTTPError_Code_TFTP_NOSUCHUSER,         /* 74 - No such user */
        HTTPError_Code_CONV_FAILED,             /* 75 - conversion failed */
        HTTPError_Code_CONV_REQD,               /* 76 - caller must register conversion
                                        callbacks using curl_easy_setopt options
                                        CURLOPT_CONV_FROM_NETWORK_FUNCTION,
                                        CURLOPT_CONV_TO_NETWORK_FUNCTION, and
                                        CURLOPT_CONV_FROM_UTF8_FUNCTION */
        HTTPError_Code_SSL_CACERT_BADFILE,      /* 77 - could not load CACERT file, missing
                                        or wrong format */
        HTTPError_Code_REMOTE_FILE_NOT_FOUND,   /* 78 - remote file not found */
        HTTPError_Code_SSH,                     /* 79 - error from the SSH layer, somewhat
                                        generic so the error message will be of
                                        interest when this has happened */
        
        HTTPError_Code_SSL_SHUTDOWN_FAILED,     /* 80 - Failed to shut down the SSL
                                        connection */
        HTTPError_Code_AGAIN,                   /* 81 - socket is not ready for send/recv,
                                        wait till it's ready and try again (Added
                                        in 7.18.2) */
        HTTPError_Code_SSL_CRL_BADFILE,         /* 82 - could not load CRL file, missing or
                                        wrong format (Added in 7.19.0) */
        HTTPError_Code_SSL_ISSUER_ERROR,        /* 83 - Issuer check failed.  (Added in
                                        7.19.0) */
        HTTPError_Code_FTP_PRET_FAILED,         /* 84 - a PRET command failed */
        HTTPError_Code_RTSP_CSEQ_ERROR,         /* 85 - mismatch of RTSP CSeq numbers */
        HTTPError_Code_RTSP_SESSION_ERROR,      /* 86 - mismatch of RTSP Session Ids */
        HTTPError_Code_FTP_BAD_FILE_LIST,       /* 87 - unable to parse FTP file list */
        HTTPError_Code_CHUNK_FAILED,            /* 88 - chunk callback reported error */
        HTTPError_Code_NO_CONNECTION_AVAILABLE, /* 89 - No connection available, the
                                        session will be queued */
        HTTPError_Code_SSL_PINNEDPUBKEYNOTMATCH, /* 90 - specified pinned public key did not
                                         match */
        HTTPError_Code_SSL_INVALIDCERTSTATUS,   /* 91 - invalid certificate status */
        HTTPError_Code_HTTP2_STREAM,            /* 92 - stream error in HTTP/2 framing layer
                                        */
        HTTPError_Code_LAST /* never use! */
    } HTTPError_Code;
    
    HTTPError(HTTPError_Code code, const std::string &error_string);
    
    HTTPError_Code getCode() const;
    const std::string getString() const;
    
private:
    HTTPError_Code _code;
    std::string _error_string;
    
public:
    static HTTPError noError();
};

#endif /* HTTPError_hpp */
