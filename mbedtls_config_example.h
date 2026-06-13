/* Mbed TLS configuration for Pico HTTPS example ******************************
 *                                                                            *
 *  Configuration for the Mbed TLS library included in the Pico SDK and       *
 *  required for the Pico HTTPS example.                                      *
 *                                                                            *
 *  N.b. Not all options are strictly required; this is just an example       *
 *  configuration.                                                            *
 *                                                                            *
 *  https://github.com/Mbed-TLS/mbedtls/blob/v2.28.2/include/mbedtls/config.h *
 *                                                                            *
 ******************************************************************************/



/* Misc **********************************************************************/

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

// Workaround for some Mbed TLS source files using INT_MAX without including limits.h
#include <limits.h>


/* System ********************************************************************/

#define MBEDTLS_HAVE_TIME

#define MBEDTLS_PLATFORM_MS_TIME_ALT
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

/* Mbed TLS features *********************************************************/

// Entropy
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT

// Symmetric ciphers
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CFB
#define MBEDTLS_CIPHER_MODE_CTR
#define MBEDTLS_CIPHER_MODE_OFB
#define MBEDTLS_CIPHER_MODE_XTS
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS

// Weak cipher suite removal
#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
#define MBEDTLS_REMOVE_3DES_CIPHERSUITES

// Elliptic curves
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_DP_CURVE448_ENABLED
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECDSA_DETERMINISTIC

// Key exchange
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED

// PKCS
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21

// TLS records
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_RECORD_CHECKING

// TLS extensions
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_SERVER_NAME_INDICATION

// Protocols
#define MBEDTLS_SSL_PROTO_TLS1_2

// X.509
#define MBEDTLS_X509_CHECK_KEY_USAGE
#define MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE



/* Modules *******************************************************************/

// Ciphers
#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_GCM_C

// Parsers
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PK_PARSE_C

// Hashing
#define MBEDTLS_MD_C
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_POLY1305_C

// Elliptic curves
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C

// RSA
#define MBEDTLS_RSA_C

// Public Key
#define MBEDTLS_PK_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS12_C

// SSL/TLS
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_CLI_C

// X.509 certificates
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C

// Requirements
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_OID_C
#define MBEDTLS_ASN1_WRITE_C

// Misc
#define MBEDTLS_ERROR_C
#define MBEDTLS_PLATFORM_C

// Debug
#define MBEDTLS_DEBUG_C
#define MBEDTLS_SSL_DEBUG_ALL
#define MBEDTLS_SSL_MAX_CONTENT_LEN 8192

#define MBEDTLS_MPI_MAX_SIZE 512

/* Module config *************************************************************/

#endif