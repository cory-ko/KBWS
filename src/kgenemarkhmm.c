#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"


int main(int argc, char **argv) {

  embInitPV("kgenemarkhmm", argc, argv, "KBWS", "1.0.3");

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPFile    outf;
  AjPStr     substr;

  AjPStr     multi = NULL;

  AjPStr     org;
  AjPStr     title;
  AjBool     list;
  AjBool     rbs;

  struct soap soap;
  struct ns1__genemarkhmmInputParams params;
  char* jobid;
  char* result;

  int i;
  for(i=0; i<argc; i++) {
    if (strcmp(argv[i], "-list") == 0) {
      list = TRUE;
    }
  }

  if (list) {

    soap_init(&soap);

    char* in0;
    if(soap_call_ns1__runGenemarkhmm( &soap, NULL, NULL, in0, &params, &jobid )== SOAP_OK) {
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

  } else {

    params.list = xsd__boolean__false_;

    org   = ajAcdGetString("org");
    title = ajAcdGetString("title");
    rbs   = ajAcdGetBoolean("rbs");

    params.org = ajCharNewS(org);
    params.title = ajCharNewS(title);
    if (rbs) {
      params.rbs = xsd__boolean__true_;
    } else {
      params.rbs = xsd__boolean__false_;
    }

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

      if(soap_call_ns1__runGenemarkhmm( &soap, NULL, NULL, in0, &params, &jobid )== SOAP_OK) {
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
      fprintf(stderr, "\n");

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

  }

  ajFileClose(&outf);
  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);
  
  embExit();

  return 0;
}

