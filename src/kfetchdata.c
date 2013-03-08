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
  embInitPV("kfetchdata", argc, argv, "KBWS", "1.0.9");
    
  struct soap soap;
  struct ns1__fetchDataInputParams params;
  char* jobid;
  char* result;
  AjPStr substr;

  AjPStr id;
  AjPFile outf;

  id=   ajAcdGetString("id");
  outf= ajAcdGetOutfile("outfile");

  // get/set parameters
  params.format = ajCharNewS(ajAcdGetString("format"));
  params.style  = ajCharNewS(ajAcdGetString("style"));

  soap_init(&soap);

  char* in0;
  in0= ajCharNewS(id);
  if (soap_call_ns1__runFetchData(&soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
  } else {
    soap_print_fault(&soap, stderr);
  }

  // polling
  int check = 0;
  while (check == 0) {
    if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid, &check) == SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr);
    }
    sleep(3);
  }

  if (soap_call_ns1__getResult(&soap, NULL, NULL, jobid, &result) == SOAP_OK) {
    substr= ajStrNewC(result);
    ajFmtPrintF(outf, "%S\n", substr);
  } else {
    soap_print_fault(&soap, stderr);
  }

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajStrDel(&id);
  ajFileClose(&outf);
  ajStrDel(&substr);

  embExit();

  return 0;
}
