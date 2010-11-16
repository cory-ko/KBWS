#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("kshowblastdb", argc, argv, "KBWS", "1.0.5");

    struct soap soap;
    struct ns1__showBlastDBInputParams params;

    char*  jobid;
    char*  result;

    AjBool  nuc;
    AjBool  prot;
    AjPFile outf;
    AjPStr  substr;

    nuc  = ajAcdGetBoolean("nucleotide");
    prot = ajAcdGetBoolean("protein");
    outf = ajAcdGetOutfile("outfile");

    if (nuc) {
      params.nucleotide = xsd__boolean__true_;
    } else {
      params.nucleotide = xsd__boolean__false_;
    }

    if (prot) {
      params.protein = xsd__boolean__true_;
    } else {
      params.protein = xsd__boolean__false_;
    }

    soap_init(&soap);

    if ( soap_call_ns1__showBlastDB( &soap, NULL, NULL, &params, &result ) == SOAP_OK ) {
      substr = ajStrNewC(result);
      ajFmtPrintF(outf,"%S\n",substr);
    } else {
      soap_print_fault(&soap, stderr);
    }
    
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

    ajStrDel(&substr);
    ajFileClose(&outf);
    embExit();

    return 0;
}
