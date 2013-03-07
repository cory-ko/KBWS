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
  // initialize EMBASSY info
  embInitPV("kweblogo", argc, argv, "KBWS", "1.0.8");

  // soap driver and parameter object
  struct soap soap;
  struct ns1__weblogoInputParams params;

  char* jobid;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    substr;
  AjPStr    inseq = NULL;
  AjPStr    format;

  // get input sequence
  seqall= ajAcdGetSeqall("seqall");


  // get/set parameters
  format= ajAcdGetString("format");
  params.format = ajCharNewS(format);

  AjPStr     tmp= NULL;
  AjPStr     tmpFileName= NULL;
  AjPSeqout  fil_file;
  AjPStr     line= NULL; /* if "AjPStr line; -> ajReadline is not success!" */
  AjPStr sizestr= NULL;
  ajint thissize;

  ajint   nb= 0;
  AjBool  are_prot= ajFalse;
  ajint   size= 0;
  AjPFile infile;

  AjPFile goutf;
  AjPStr  goutfile;

  goutfile= ajAcdGetString("goutfile");

  tmp= ajStrNewC("fasta");

  fil_file= ajSeqoutNew();
  tmpFileName= getUniqueFileName();

  if(!ajSeqoutOpenFilename(fil_file, tmpFileName)) {
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
  in0= ajCharNewS(inseq);
  if (soap_call_ns1__runWeblogo( &soap, NULL, NULL, in0, &params, &jobid) == SOAP_OK) {
  } else {
    soap_print_fault(&soap, stderr);
  }

  int check= 0;
  while (check == 0 ) {
    if (soap_call_ns1__checkStatus(&soap, NULL, NULL, jobid,  &check) == SOAP_OK) {
    } else {
      soap_print_fault(&soap, stderr);
    }
    sleep(3);
  }

  char* image_url;
  if (soap_call_ns1__getResult(&soap, NULL, NULL, jobid,  &image_url) == SOAP_OK) {
    goutf= ajFileNewOutNameS(goutfile);

    if (!goutf) {
      // can not open image output file
      ajFmtError("Problem writing out image file");
      embExitBad();
    }

    if (!gHttpGetBinC(image_url, &goutf)) {
      // can not download image file
      ajFmtError("Problem downloading image file");
      embExitBad();
    }
  } else {
    soap_print_fault(&soap, stderr);
  }

  // delete temporary multi-fasta sequence file
  ajSysFileUnlinkS(tmpFileName);

  // destruct SOAP driver
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  // destruct EMBOSS object
  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&substr);

  // exit
  embExit();

  return 0;
}

static AjPStr getUniqueFileName(void) {
  static char ext[2]= "A";
  AjPStr filename= NULL;

  ajFmtPrintS(&filename, "%08d%s", getpid(), ext);

  if (++ext[0] > 'Z') {
    ext[0]= 'A';
  }

  return filename;
}
