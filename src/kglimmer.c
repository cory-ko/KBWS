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
  embInitPV("kglimmer", argc, argv, "KBWS", "1.0.9");

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPFile    outf;
  AjPStr     substr;

  AjPStr     multi = NULL;

  AjPStr     topology;
  ajint      gencode;

  struct soap soap;
  struct ns1__glimmerInputParams params;
  char* jobid;
  char* result;

  topology = ajAcdGetString("topology");
  gencode  = ajAcdGetInt("gencode");

  params.topology = ajCharNewS(topology);
  params.gencode  = gencode;

  seqall = ajAcdGetSeqall("seqall");
  outf   = ajAcdGetOutfile("outfile");

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    multi = NULL;
    ajStrAppendC(&multi,">");
    ajStrAppendS(&multi,ajSeqGetNameS(seq));
    ajStrAppendC(&multi,"\n");
    ajStrAppendS(&multi,ajSeqGetSeqS(seq));
    
    char* in0;
    in0 = ajCharNewS(multi);
      
    if(soap_call_ns1__runGlimmer( &soap, NULL, NULL, in0, &params, &jobid )== SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr); 
    }

    int check = 0;
    while (check == 0) {
      if(soap_call_ns1__checkStatus( &soap, NULL, NULL, jobid,  &check )== SOAP_OK) {
      } else {
	soap_print_fault(&soap, stderr); 
      }
      sleep(3);
    }

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
