#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
  // initialize EMBASSY info
  embInitPV("kphobius", argc, argv, "KBWS", "1.0.9");

  struct soap soap;
  struct ns1__phobiusInputParams params;
  char* jobid;
  char* result;

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPFile    outf;
  AjPStr     substr;
  AjPStr     inseq = NULL;
  AjPStr     outputformat;
  outputformat =      ajAcdGetString("outputformat");

  seqall = ajAcdGetSeqall("seqall");
  outf   = ajAcdGetOutfile("outfile");
  params.outputformat = ajCharNewS(outputformat);

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    inseq = NULL;
    ajStrAppendC(&inseq,">");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    ajStrAppendC(&inseq,"\n");
    ajStrAppendS(&inseq,ajSeqGetSeqS(seq));

    char* in0;
    in0 = ajCharNewS(inseq);
    if ( soap_call_ns1__runPhobius( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
    } else {
      soap_print_fault(&soap, stderr);
    }

    int check = 0;
    while ( check == 0 ) {
      if ( soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check ) == SOAP_OK ) {
      } else {
	soap_print_fault(&soap, stderr);
      }
      sleep(3);
    }

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
