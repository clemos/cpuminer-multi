#ifndef __JSON_API_H__
#define __JSON_API_H__

#include <jansson.h>
#include <curl/curl.h>
#include <openssl/sha.h>

#if JANSSON_MAJOR_VERSION >= 2
#define JSON_LOADS(str, err_ptr) json_loads(str, 0, err_ptr)
#define JSON_LOADF(path, err_ptr) json_load_file(path, 0, err_ptr)
#else
#define JSON_LOADS(str, err_ptr) json_loads(str, err_ptr)
#define JSON_LOADF(path, err_ptr) json_load_file(path, err_ptr)
#endif

json_t* json_load_url(char* cfg_url, json_error_t *err);

#define JSON_RPC_LONGPOLL (1 << 0)
#define JSON_RPC_QUIET_404  (1 << 1)
#define JSON_RPC_IGNOREERR  (1 << 2)

#define JSON_BUF_LEN 512

extern json_t *json_rpc_call(CURL *curl, const char *url, const char *userpass,
  const char *rpc_req, int *curl_err, int flags);

bool jobj_binary(const json_t *obj, const char *key, void *buf, size_t buflen);

struct stratum_job {
  char *job_id;
  unsigned char prevhash[32];
  size_t coinbase_size;
  unsigned char *coinbase;
  unsigned char *xnonce2;
  int merkle_count;
  unsigned char **merkle;
  unsigned char version[4];
  unsigned char nbits[4];
  unsigned char ntime[4];
  unsigned char claim[32]; // lbry
  bool clean;
  double diff;
};

struct stratum_ctx {
  char *url;

  CURL *curl;
  char *curl_url;
  char curl_err_str[CURL_ERROR_SIZE];
  curl_socket_t sock;
  size_t sockbuf_size;
  char *sockbuf;
  pthread_mutex_t sock_lock;

  double next_diff;
  double sharediff;

  char *session_id;
  size_t xnonce1_size;
  unsigned char *xnonce1;
  size_t xnonce2_size;
  struct stratum_job job;
  struct work work;
  pthread_mutex_t work_lock;

  int bloc_height;
};

bool stratum_socket_full(struct stratum_ctx *sctx, int timeout);
bool stratum_send_line(struct stratum_ctx *sctx, char *s);
char *stratum_recv_line(struct stratum_ctx *sctx);
bool stratum_connect(struct stratum_ctx *sctx, const char *url);
void stratum_disconnect(struct stratum_ctx *sctx);
bool stratum_subscribe(struct stratum_ctx *sctx);
bool stratum_authorize(struct stratum_ctx *sctx, const char *user, const char *pass);
bool stratum_handle_method(struct stratum_ctx *sctx, const char *s);

/* rpc 2.0 (xmr) */
extern bool jsonrpc_2;
extern char rpc2_id[64];
extern char *rpc2_blob;
extern size_t rpc2_bloblen;
extern uint32_t rpc2_target;
extern char *rpc2_job_id;

json_t *json_rpc2_call(CURL *curl, const char *url, const char *userpass, const char *rpc_req, int *curl_err, int flags);
bool rpc2_login(CURL *curl);
bool rpc2_login_decode(const json_t *val);
bool rpc2_workio_login(CURL *curl);
bool rpc2_stratum_job(struct stratum_ctx *sctx, json_t *params);
bool rpc2_job_decode(const json_t *job, struct work *work);

void parse_config(json_t *config, char *ref);

#endif /* __JSON_API_H__ */
