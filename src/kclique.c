#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

static AjPStr getUniqueFileName(void);

int main(int argc, char **argv) {

  embInitPV("kclique", argc, argv, "KBWS", "1.0.2");

  struct soap soap;
  char*  jobid;
  char*  result;

  AjPFile    infile;
  AjPFile    outf;
  AjPStr     substr;
  AjPStr     indata = NULL;
  AjPStr     line   = NULL;

  infile = ajAcdGetInfile("infile");
  outf   = ajAcdGetOutfile("outfile");

  while (ajReadline(infile, &line)) {
    ajStrAppendS(&indata, line);
    ajStrAppendC(&indata, "\n");
  }

  soap_init(&soap);

  char* in0;
  in0 = ajCharNewS(indata);
  if ( soap_call_ns1__runClique( &soap, NULL, NULL, in0, &jobid ) == SOAP_OK ) {
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

  ajFileClose(&infile);
  ajFileClose(&outf);
  ajStrDel(&substr);

  embExit();

  return 0;
}

