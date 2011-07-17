#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("kcentroidfold", argc, argv, "KBWS", "1.0.8");

    struct soap soap;
    struct ns1__centroidfoldInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     model;
    ajint      gamma;
    model =      ajAcdGetString("model");
    gamma =      ajAcdGetInt("gamma");

    seqall       = ajAcdGetSeqall("seqall");
    outf         = ajAcdGetOutfile("outfile");
    params.model = ajCharNewS(model);
    params.gamma = gamma;

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
      if ( soap_call_ns1__runCentroidfold( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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

      char* type;
      type = "out";
      if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
	substr = ajStrNewC(result);
	ajFmtPrintF(outf,"%S\n",substr);
      } else {
	soap_print_fault(&soap, stderr); 
      }

      /*
      type = "png";
      if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
	substr = ajStrNewC(result);
	
	ajFmtPrintF(outf_png,"%S\n",substr);
      } else {
	soap_print_fault(&soap, stderr); 
      }
      */

      fprintf(stdout, "http://soap.g-language.org/result/");
      fprintf(stdout, "%s", jobid);
      fprintf(stdout, ".png\n");

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
