#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("kblast", argc, argv, "KBWS", "1.0.3");

    struct soap soap;
    struct ns1__blastInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     p;
    AjPStr     d;
    AjPStr     server;
    AjPStr     m;
    float      e;
    AjPStr     F;
    ajint      G;
    ajint      E;
    AjPStr     X;
    ajint      q;
    ajint      r;
    ajint      v;
    ajint      b;
    ajint      f;
    AjBool     g;
    AjPStr     M;
    ajint      W;
    float      z;
    ajint      K;
    float      Y;
    p      =      ajAcdGetString("p");
    d      =      ajAcdGetString("d");
    server =      ajAcdGetString("server");
    m      =      ajAcdGetString("format");
    e      =      ajAcdGetFloat("eval");
    F      =      ajAcdGetString("qfilter");
    G      =      ajAcdGetInt("op");
    E      =      ajAcdGetInt("ep");
    X      =      ajAcdGetString("dropoff");
    q      =      ajAcdGetInt("q");
    r      =      ajAcdGetInt("r");
    v      =      ajAcdGetInt("v");
    b      =      ajAcdGetInt("b");
    f      =      ajAcdGetInt("threshold");
    g      =      ajAcdGetBoolean("g");
    M      =      ajAcdGetString("matrix");
    W      =      ajAcdGetInt("wordsize");
    z      =      ajAcdGetFloat("z");
    K      =      ajAcdGetInt("k");
    Y      =      ajAcdGetFloat("y");

    seqall        = ajAcdGetSeqall("seqall");
    outf          = ajAcdGetOutfile("outfile");
    params.p      = ajCharNewS(p);
    params.d      = ajCharNewS(d);
    params.server = ajCharNewS(server);
    params.m      = ajCharNewS(m);
    params.e      = e;
    params.F      = ajCharNewS(F);
    params.G      = G;
    params.E      = E;
    params.X      = ajCharNewS(X);
    params.q      = q;
    params.r      = r;
    params.v      = v;
    params.b      = b;
    params.f      = f;
    if (g) {
      params.g    = xsd__boolean__true_;
    } else {
      params.g    = xsd__boolean__false_;
    }
    params.M      = ajCharNewS(M);
    params.W      = W;
    params.z      = z;
    params.K      = K;
    params.Y      = Y;

    while (ajSeqallNext(seqall, &seq)) {

      soap_init(&soap);

      inseq = NULL;
      ajStrAppendC(&inseq,">");
      ajStrAppendS(&inseq,ajSeqGetNameS(seq));
      ajStrAppendC(&inseq,"\n");
      ajStrAppendS(&inseq,ajSeqGetSeqS(seq));

      soap_init(&soap);

      char* in0;
      in0 = ajCharNewS(inseq);
      if ( soap_call_ns1__runBlast( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
          fprintf(stderr,"Jobid: %s\n",jobid);
      } else {
          soap_print_fault(&soap, stderr);
      }

      int check = 0;
      while ( check == 0 ) {
          if ( soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check ) == SOAP_OK ) {
              fprintf(stderr,"*");
          } else {
              soap_print_fault(&soap, stderr);
          }
          sleep(3);
      }
      fprintf(stderr,"\n");

      if ( soap_call_ns1__getResult( &soap, NULL, NULL, jobid,  &result ) == SOAP_OK ) {
	    substr = ajStrNewC(result);
          ajFmtPrintF(outf,"%S\n",substr);
      } else {
          soap_print_fault(&soap, stderr);
      }

      soap_destroy(&soap);
      soap_end(&soap);
      soap_done(&soap);

    }

    ajFileClose(&outf);

    ajSeqallDel(&seqall);
    ajSeqDel(&seq);
    ajStrDel(&substr);

    embExit();

    return 0;
}

