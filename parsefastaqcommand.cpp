/*
 *  parsefastaqcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/30/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "parsefastaqcommand.h"
#include "sequence.hpp"

//**********************************************************************************************************************
vector<string> ParseFastaQCommand::setParameters(){	
	try {
		CommandParameter pfastq("fastq", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(pfastq);
		CommandParameter pfasta("fasta", "Boolean", "", "T", "", "", "","fasta",false,false); parameters.push_back(pfasta);
		CommandParameter pqual("qfile", "Boolean", "", "T", "", "", "","qfile",false,false); parameters.push_back(pqual);
        CommandParameter ppacbio("pacbio", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(ppacbio);
 		CommandParameter pformat("format", "Multiple", "sanger-illumina-solexa-illumina1.8+", "sanger", "", "", "","",false,false,true); parameters.push_back(pformat);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseFastaQCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The fastq.info command reads a fastq file and creates a fasta and quality file.\n";
		helpString += "The fastq.info command parameters are fastq, fasta, qfile and format; fastq is required.\n";
        helpString += "The fastq.info command should be in the following format: fastq.info(fastaq=yourFastaQFile).\n";
		helpString += "The format parameter is used to indicate whether your sequences are sanger, solexa, illumina1.8+ or illumina, default=sanger.\n";
        helpString += "The fasta parameter allows you to indicate whether you want a fasta file generated. Default=T.\n";
        helpString += "The qfile parameter allows you to indicate whether you want a quality file generated. Default=T.\n";
        helpString += "The pacbio parameter allows you to indicate .... When set to true, quality scores of 0 will results in a corresponding base of N. Default=F.\n";
		helpString += "Example fastq.info(fastaq=test.fastaq).\n";
		helpString += "Note: No spaces between parameter labels (i.e. fastq), '=' and yourFastQFile.\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseFastaQCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "fasta") {  pattern = "[filename],fasta"; } 
        else if (type == "qfile") {  pattern = "[filename],qual"; } 
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->control_pressed = true;  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ParseFastaQCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
ParseFastaQCommand::ParseFastaQCommand(){	
	try {
		abort = true; calledHelp = true; 
		setParameters();
		vector<string> tempOutNames;
		outputTypes["fasta"] = tempOutNames;
		outputTypes["qfile"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "ParseFastaQCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
ParseFastaQCommand::ParseFastaQCommand(string option){
	try {
		abort = false; calledHelp = false;   
		
		if(option == "help") {	help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;

			//check to make sure all parameters are valid for command
			for (map<string,string>::iterator it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["fasta"] = tempOutNames;
			outputTypes["qfile"] = tempOutNames;
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("fastq");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["fastq"] = inputDir + it->second;		}
				}
			}
			
			//check for required parameters
			fastaQFile = validParameter.validFile(parameters, "fastq", true);
			if (fastaQFile == "not found") {	m->mothurOut("fastq is a required parameter for the fastq.info command.");	m->mothurOutEndLine();	abort = true;	}
			else if (fastaQFile == "not open")	{	fastaQFile = ""; abort = true;	}	
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);	if (outputDir == "not found"){	outputDir = m->hasPath(fastaQFile); 	}
			
			string temp;
			temp = validParameter.validFile(parameters, "fasta", false);	if(temp == "not found"){	temp = "T";	}
			fasta = m->isTrue(temp); 

			temp = validParameter.validFile(parameters, "qfile", false);	if(temp == "not found"){	temp = "T";	}
			qual = m->isTrue(temp);
            
            temp = validParameter.validFile(parameters, "pacbio", false);	if(temp == "not found"){	temp = "F";	}
			pacbio = m->isTrue(temp);

			
            format = validParameter.validFile(parameters, "format", false);		if (format == "not found"){	format = "sanger";	}
            
            if ((format != "sanger") && (format != "illumina") && (format != "illumina1.8+") && (format != "solexa"))  { 
				m->mothurOut(format + " is not a valid format. Your format choices are sanger, solexa, illumina1.8+ and illumina, aborting." ); m->mothurOutEndLine();
				abort=true;
			}

			if ((!fasta) && (!qual)) { m->mothurOut("[ERROR]: no outputs selected. Aborting."); m->mothurOutEndLine(); abort=true; }

		}		
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "ParseFastaQCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int ParseFastaQCommand::execute(){
	try {
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
		
		//open Output Files
        map<string, string> variables; 
        variables["[filename]"] = outputDir + m->getRootName(m->getSimpleName(fastaQFile));
		string fastaFile = getOutputFileName("fasta",variables);
		string qualFile = getOutputFileName("qfile",variables);
		ofstream outFasta, outQual;
		
		if (fasta) { m->openOutputFile(fastaFile, outFasta);  outputNames.push_back(fastaFile); outputTypes["fasta"].push_back(fastaFile);	}
		if (qual) { m->openOutputFile(qualFile, outQual);	outputNames.push_back(qualFile);  outputTypes["qfile"].push_back(qualFile);		}
		
		ifstream in;
		m->openInputFile(fastaQFile, in);
        
        //fill convert table - goes from solexa to sanger. Used fq_all2std.pl as a reference.
        for (int i = -64; i < 65; i++) { 
            char temp = (char) ((int)(33 + 10*log(1+pow(10,(i/10.0)))/log(10)+0.499));
            convertTable.push_back(temp);
        }
		
		while (!in.eof()) {
			
			if (m->control_pressed) { break; }
		
			//read sequence name
			string name = m->getline(in); m->gobble(in);
			if (name == "") {  m->mothurOut("[ERROR]: Blank fasta name."); m->mothurOutEndLine(); m->control_pressed = true; break; }
			else if (name[0] != '@') { m->mothurOut("[ERROR]: reading " + name + " expected a name with @ as a leading character."); m->mothurOutEndLine(); m->control_pressed = true; break; }
			else { 
                name = name.substr(1); 
                m->checkName(name);
            }
			
			//read sequence
			string sequence = m->getline(in); m->gobble(in);
			if (sequence == "") {  m->mothurOut("[ERROR]: missing sequence for " + name); m->mothurOutEndLine(); m->control_pressed = true; break; }
			
			//read sequence name
			string name2 = m->getline(in); m->gobble(in);
			if (name2 == "") {  m->mothurOut("[ERROR]: Blank quality name."); m->mothurOutEndLine(); m->control_pressed = true; break; }
			else if (name2[0] != '+') { m->mothurOut("[ERROR]: reading " + name2 + " expected a name with + as a leading character."); m->mothurOutEndLine(); m->control_pressed = true; break; }
			else { 
                name2 = name2.substr(1);  
                m->checkName(name2);
            }
			
			//read quality scores
			string quality = m->getline(in); m->gobble(in);
			if (quality == "") {  m->mothurOut("[ERROR]: missing quality for " + name2); m->mothurOutEndLine(); m->control_pressed = true; break; }
			
			//sanity check sequence length and number of quality scores match
			if (name2 != "") { if (name != name2) { m->mothurOut("[ERROR]: names do not match. read " + name + " for fasta and " + name2 + " for quality."); m->mothurOutEndLine(); m->control_pressed = true; break; } }
			if (quality.length() != sequence.length()) { m->mothurOut("[ERROR]: Lengths do not match for sequence " + name + ". Read " + toString(sequence.length()) + " characters for fasta and " + toString(quality.length()) + " characters for quality scores."); m->mothurOutEndLine(); m->control_pressed = true; break; }
			
            vector<int> qualScores;
            if (qual) {
				qualScores = convertQual(quality);
				outQual << ">" << name << endl;
				for (int i = 0; i < qualScores.size(); i++) { outQual << qualScores[i] << " "; }
				outQual << endl;
			}
            
            if (m->control_pressed) { break; }
            
            if (pacbio) {
                if (!qual) { qualScores = convertQual(quality); } //get scores if we didn't already
                for (int i = 0; i < qualScores.size(); i++) {
                    if (qualScores[i] == 0){ sequence[i] = 'N'; }
                }
            }
            
			//print sequence info to files
			if (fasta) { outFasta << ">" << name << endl << sequence << endl; }
			
		}
		
		in.close();
		if (fasta)	{ outFasta.close();	}
		if (qual)	{ outQual.close();	}
		
		if (m->control_pressed) { outputTypes.clear(); outputNames.clear(); m->mothurRemove(fastaFile); m->mothurRemove(qualFile); return 0; }
		
		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("fasta");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setFastaFile(current); }
		}
		
		itTypes = outputTypes.find("qfile");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setQualFile(current); }
		}		
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<int> ParseFastaQCommand::convertQual(string qual) {
	try {
		vector<int> qualScores;
		
        bool negativeScores = false;
        
		for (int i = 0; i < qual.length(); i++) { 
            
            int temp = 0;
            temp = int(qual[i]);
            if (format == "illumina") {
                temp -= 64; //char '@'
            }else if (format == "illumina1.8+") {
                temp -= int('!'); //char '!'
            }else if (format == "solexa") {
                temp = int(convertTable[temp]); //convert to sanger
                temp -= int('!'); //char '!'
            }else {
                temp -= int('!'); //char '!'
            }
            if (temp < -5) { negativeScores = true; }
			qualScores.push_back(temp);
		}
		
        if (negativeScores) { m->mothurOut("[ERROR]: finding negative quality scores, do you have the right format selected? http://en.wikipedia.org/wiki/FASTQ_format#Encoding \n");  m->control_pressed = true;  }
        
		return qualScores;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseFastaQCommand", "convertQual");
		exit(1);
	}
}
//**********************************************************************************************************************



