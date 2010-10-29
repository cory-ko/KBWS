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

  embInitPV("kclustalw", argc, argv, "KBWS", "1.0.2");

  struct soap soap;
  struct ns1__clustalwInputParams params;
  char* jobid;
  char* result;

  AjPSeqall  seqall;
  AjPSeq     seq;
  AjPFile    outf;
  AjPFile    outf_dnd;
  AjPStr     substr;
  AjPStr     inseq = NULL;
  AjPStr     alignment;
  AjPStr     output;
  AjPStr     matrix;
  AjPStr     outorder;
  ajint      ktup;
  ajint      window;
  ajint      gapopen;
  float      gapext;
  ajint      gapdist;
  AjBool     endgaps;
  ajint      pairgap;
  ajint      topdiags;
  AjPStr     score;
  AjBool     tossgaps;
  AjBool     kimura;
  AjPStr     outputtree;
  AjBool     tree;
  AjBool     quicktree;
  AjBool     align;
  AjPStr     clustering;
  ajint      numiter;
  AjPStr     iteration;
  alignment  = ajAcdGetString("alignment");
  output     = ajAcdGetString("output");
  matrix     = ajAcdGetString("matrix");
  outorder   = ajAcdGetString("outorder");
  ktup       = ajAcdGetInt("ktup");
  window     = ajAcdGetInt("window");
  gapopen    = ajAcdGetInt("gapopen");
  gapext     = ajAcdGetFloat("gapext");
  gapdist    = ajAcdGetInt("gapdist");
  endgaps    = ajAcdGetBoolean("endgaps");
  pairgap    = ajAcdGetInt("pairgap");
  topdiags   = ajAcdGetInt("topdiags");
  score      = ajAcdGetString("score");
  tossgaps   = ajAcdGetBoolean("tossgaps");
  kimura     = ajAcdGetBoolean("kimura");
  outputtree = ajAcdGetString("outputtree");
  tree       = ajAcdGetBoolean("tree");
  quicktree  = ajAcdGetBoolean("quicktree");
  align      = ajAcdGetBoolean("align");
  clustering = ajAcdGetString("clustering");
  numiter    = ajAcdGetInt("numiter");
  iteration  = ajAcdGetString("iteration");

  seqall = ajAcdGetSeqall("seqall");
  outf   = ajAcdGetOutfile("outfile");
  outf_dnd = ajAcdGetOutfile("dndoutfile");
  params.alignment = ajCharNewS(alignment);
  params.output = ajCharNewS(output);
  params.matrix = ajCharNewS(matrix);
  params.outorder = ajCharNewS(outorder);
  params.ktup = ktup;
  params.window = window;
  params.gapopen = gapopen;
  params.gapext = gapext;
  params.gapdist = gapdist;
  if (endgaps) {
    params.endgaps = xsd__boolean__true_;
  } else {
    params.endgaps = xsd__boolean__false_;
  }
  params.pairgap = pairgap;
  params.topdiags = topdiags;
  params.score = ajCharNewS(score);

  if (tossgaps) {
    params.tossgaps = xsd__boolean__true_;
  } else {
    params.tossgaps = xsd__boolean__false_;
  }
  if (kimura) {
    params.kimura = xsd__boolean__true_;
  } else {
    params.kimura = xsd__boolean__false_;
  }
  params.outputtree = ajCharNewS(outputtree);
  if (tree) {
    params.tree = xsd__boolean__true_;
  } else {
    params.tree = xsd__boolean__false_;
  }
  if (quicktree) {
    params.quicktree = xsd__boolean__true_;
  } else {
    params.quicktree = xsd__boolean__false_;
  }
  if (align) {
    params.align = xsd__boolean__true_;
  } else {
    params.align = xsd__boolean__false_;
  }
  params.clustering = ajCharNewS(clustering);
  params.numiter = numiter;
  params.iteration = ajCharNewS(iteration);

  AjPStr     tmp = NULL;
  AjPStr     tmpFileName = NULL;
  AjPSeqout  fil_file;
  AjPStr     line = NULL; /* if "AjPStr line; -> ajReadline is not success!" */
  AjPStr sizestr = NULL;
  ajint thissize;

  ajint      nb = 0;
  AjBool     are_prot = ajFalse;
  ajint      size = 0;
  AjPFile    infile;

  tmp = ajStrNewC("fasta");

  fil_file = ajSeqoutNew();
  tmpFileName = getUniqueFileName();

  if( !ajSeqoutOpenFilename(fil_file, tmpFileName) ) {
    embExitBad();
  }

  ajSeqoutSetFormatS(fil_file, tmp);

  while (ajSeqallNext(seqall, &seq)) {
    if (!nb) {
      are_prot  = ajSeqIsProt(seq);
    }
    ajSeqoutWriteSeq(fil_file, seq);
    ++nb;
  }
  ajSeqoutClose(fil_file);
  ajSeqoutDel(&fil_file);

  if (nb < 2) {
    ajFatal("Multiple alignments need at least two sequences");
  }

  infile = ajFileNewInNameS(tmpFileName);

  while (ajReadline(infile, &line)) {
    ajStrAppendS(&inseq,line);
    ajStrAppendC(&inseq,"\n");
  }

  soap_init(&soap);

  char* in0;
  in0 = ajCharNewS(inseq);
  if ( soap_call_ns1__runClustalw( &soap, NULL, NULL, in0, &params, &jobid ) == SOAP_OK ) {
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
    fprintf(stdout, "%s\n", ajStrGetPtr(substr));
  } else {
    soap_print_fault(&soap, stderr);
  }

  type = "aln";
  if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
    substr = ajStrNewC(result);
    ajFmtPrintF(outf,"%S\n",substr);
  } else {
    soap_print_fault(&soap, stderr); 
  }

  type = "dnd";
  if(soap_call_ns1__getMultiResult( &soap, NULL, NULL, jobid, type, &result )== SOAP_OK) {
    substr = ajStrNewC(result);
    ajFmtPrintF(outf_dnd,"%S\n",substr);
  } else {
    soap_print_fault(&soap, stderr); 
  }


  ajSysFileUnlink(tmpFileName);

  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  ajFileClose(&outf_dnd);

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);

  embExit();

  return 0;
}

static AjPStr getUniqueFileName(void) {
  static char ext[2] = "A";
  AjPStr filename    = NULL;

  ajFmtPrintS(&filename, "%08d%s",getpid(), ext);

  if( ++ext[0] > 'Z' ) {
    ext[0] = 'A';
  }

  return filename;
}
