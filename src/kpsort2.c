#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;

    embInitPV("kpsort2", argc, argv, "KBWS", "1.0.4");

    struct soap soap;

    char* jobid;
    char* result;

    seqall = ajAcdGetSeqall("seqall");
    outf   = ajAcdGetOutfile("outfile");

    while(ajSeqallNext(seqall, &seq)) {

      soap_init(&soap);

      char* in0;
      in0 = ajSeqGetSeqCopyC(seq);
      //ajSeqGetNameC(seq);

      if(soap_call_ns1__runPsort2( &soap, NULL, NULL, in0, &jobid )== SOAP_OK) {
	fprintf(stderr,"Jobid: %s\n",jobid);
      } else {
	soap_print_fault(&soap, stderr); 
      }

      int check = 0;
      while (check == 0) {
        if(soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check )== SOAP_OK) {
          fprintf(stderr,"*");
        } else {
          soap_print_fault(&soap, stderr); 
        }
        sleep(3);
      }

      fprintf(stderr,"\n");

      if(soap_call_ns1__getResult( &soap, NULL, NULL, jobid,  &result )== SOAP_OK) {
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
