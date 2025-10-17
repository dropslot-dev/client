#include "auth_util.h"
#include "mbedtls/md.h"

String computeRoomIdHash(const String& roomId) {
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    
    const size_t hashLen = 32;
    unsigned char hash[hashLen];
    
    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char*)roomId.c_str(), roomId.length());
    mbedtls_md_finish(&ctx, hash);
    mbedtls_md_free(&ctx);
    
    String hashString = "";
    for(int i = 0; i < hashLen; i++) {
        char hex[3];
        sprintf(hex, "%02x", hash[i]);
        hashString += hex;
    }
    
    return hashString;
}
