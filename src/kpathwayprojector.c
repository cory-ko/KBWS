#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {
  // initialize EMBASSY info
  embInitPV("kpathwayprojector", argc, argv, "KBWS", "1.0.9");

  struct soap soap;
  char*  jobid;
  char*  result;

  AjPFile infile;
  AjPStr  substr;
  AjPStr  indata = NULL;
  AjPStr  line   = NULL;

  infile = ajAcdGetInfile("infile");
  while (ajReadline(infile, &line)) {
    ajStrAppendS(&indata, line);
    ajStrAppendC(&indata, "\n");
  }

  soap_init(&soap);

  char* in0;
  in0 = ajCharNewS(indata);
  if ( soap_call_ns1__map2PathwayProjector( &soap, NULL, NULL, in0, &jobid ) == SOAP_OK ) {
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
    fprintf(stdout, "Pathway Projector's URL : %s\n", ajStrGetPtr(substr));
  } else {
    soap_print_fault(&soap, stderr);
  }

  fprintf(stdout,"Image File : http://soap.g-language.org/result/");
  fprintf(stdout,jobid);
  fprintf(stdout,".png\n");

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajStrDel(&substr);

  embExit();

  return 0;
}

