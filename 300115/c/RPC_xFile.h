/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _RPC_XFILE_H_RPCGEN
#define _RPC_XFILE_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NOMEDIRETTORIO_LENGTH 50
#define PREFISSO_LENGTH 10
#define FILENAME_LENGTH 25
#define NUM_FILES 10

struct Input {
	char *nomeDirettorio;
};
typedef struct Input Input;

struct MyFile {
	char filename[FILENAME_LENGTH];
};
typedef struct MyFile MyFile;

struct OutputArray {
	int errore;
	int nFiles;
	MyFile files[NUM_FILES];
};
typedef struct OutputArray OutputArray;

#define ESAMEPROG 0x20042069
#define ESAMEVERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define LISTA_FILE 1
extern  OutputArray * lista_file_1(Input *, CLIENT *);
extern  OutputArray * lista_file_1_svc(Input *, struct svc_req *);
#define NUMERAZIONE_RIGHE 2
extern  int * numerazione_righe_1(char **, CLIENT *);
extern  int * numerazione_righe_1_svc(char **, struct svc_req *);
extern int esameprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define LISTA_FILE 1
extern  OutputArray * lista_file_1();
extern  OutputArray * lista_file_1_svc();
#define NUMERAZIONE_RIGHE 2
extern  int * numerazione_righe_1();
extern  int * numerazione_righe_1_svc();
extern int esameprog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Input (XDR *, Input*);
extern  bool_t xdr_MyFile (XDR *, MyFile*);
extern  bool_t xdr_OutputArray (XDR *, OutputArray*);

#else /* K&R C */
extern bool_t xdr_Input ();
extern bool_t xdr_MyFile ();
extern bool_t xdr_OutputArray ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_RPC_XFILE_H_RPCGEN */
