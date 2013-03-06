#include <stdio.h>
#include <stdlib.h>

#include "soapH.h"
#include "KBWSSoapBinding.nsmap"

#include "emboss.h"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char **argv) {

    embInitPV("kssearch", argc, argv, "KBWS", "1.0.8");

    struct soap soap;
    struct ns1__ssearchInputParams params;
    char* jobid;
    char* result;

    AjPSeqall  seqall;
    AjPSeq     seq;
    AjPFile    outf;
    AjPStr     substr;
    AjPStr     inseq = NULL;
    AjPStr     database;
    AjPStr     moltype;
    AjBool     histogram;
    AjBool     nucleotide;
    AjBool     topstrand;
    AjBool     bottomstrand;
    ajint      gapopen;
    ajint      gapext;
    ajint      scores;
    ajint      alignments;
    ajint      ktup;
    AjPStr     matrix;
    float      eupper;
    float      elower;
    AjPStr     dbrange;
    AjPStr     seqrange;
    database     = ajAcdGetString("database");
    moltype      = ajAcdGetString("moltype");
    histogram    = ajAcdGetBoolean("histogram");
    nucleotide   = ajAcdGetBoolean("nucleotide");
    topstrand    = ajAcdGetBoolean("topstrand");
    bottomstrand = ajAcdGetBoolean("bottomstrand");
    gapopen      = ajAcdGetInt("gapopen");
    gapext       = ajAcdGetInt("gapext");
    scores       = ajAcdGetInt("scores");
    alignments   = ajAcdGetInt("alignments");
    ktup         = ajAcdGetInt("ktup");
    matrix       = ajAcdGetString("matrix");
    eupper       = ajAcdGetFloat("eupper");
    elower       = ajAcdGetFloat("elower");
    dbrange      = ajAcdGetString("dbrange");
    seqrange     = ajAcdGetString("seqrange");

    seqall   = ajAcdGetSeqall("seqall");
    outf     = ajAcdGetOutfile("outfile");
    params.d = ajCharNewS(database);
    params.moltype = ajCharNewS(moltype);
    if (histogram) {
      params.histogram = xsd__boolean__true_;
    } else {
      params.histogram = xsd__boolean__false_;
    }
    if (nucleotide) {
      params.nucleotide = xsd__boolean__true_;
    } else {
      params.nucleotide = xsd__boolean__false_;
    }
    if (topstrand) {
      params.topstrand = xsd__boolean__true_;
    } else {
      params.topstrand = xsd__boolean__false_;
    }
    if (bottomstrand) {
      params.bottomstrand = xsd__boolean__true_;
    } else {
      params.bottomstrand = xsd__boolean__false_;
    }
    params.gapopen    = gapopen;
    params.gapext     = gapext;
    params.scores     = scores;
    params.alignments = alignments;
    params.ktup       = ktup;
    params.matrix     = ajCharNewS(matrix);
    params.eupper     = eupper;
    params.elower     = elower;
    params.dbrange    = ajCharNewS(dbrange);
    params.seqrange   = ajCharNewS(seqrange);

    while (ajSeqallNext(seqall, &seq)) {

      soap_init(&soap);

      inseq = NULL;
      ajStrAppendC(&inseq,">");
      ajStrAppendS(&inseq,ajSeqGetNameS(seq));
      ajStrAppendC(&inseq,"\n");
      ajStrAppendS(&inseq,ajSeqGetSeqS(seq));

      char* in0;
      in0 = ajCharNewS(inseq);
      if ( soap_call_ns1__runSsearch( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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

       sleep(3);

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
