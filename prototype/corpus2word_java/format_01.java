import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

public class format_01 {
	ArrayList<String> types = new ArrayList<String>();
	format_01(String inputFile){
		String FileName = inputFile.split("\\.")[0];
		File inFile = new File("input\\" + FileName + ".txt");
		File outFile = new File("output\\text88.txt");
		if(!outFile.exists()) {
			try {
				outFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		BufferedWriter bw = null;
		BufferedReader br = null;
  
		try {
			String line;
			br = new BufferedReader(new InputStreamReader(new FileInputStream(inFile), "UTF-8")); 
			bw = new BufferedWriter(new FileWriter(outFile, true));

			String Sentence = "";
			String Sentence2 = "";
			boolean sourceArea = true;
   
			while ((line = br.readLine()) != null) {
				if(line.contains("<source>")) sourceArea = true;
				else if(line.contains("</source>")) sourceArea = false;
				if(!sourceArea) {
					if(line.contains(FileName)){
						Sentence = Sentence + line.split("\t")[1] + " ";
						Sentence2 = Sentence2 + check(line.split("\t")[2]);
					}else if(line.contains("</p>") || line.contains("</head>") ){
						if(Sentence != ""){
//							System.out.println(Sentence);
//							System.out.println(Sentence2);
//							System.out.println();
//							bw.write(Sentence);
//						   	bw.newLine();
						   	bw.write(Sentence2);
						   	bw.newLine();
//						   	bw.newLine();
						   	Sentence = "";
						   	Sentence2 = "";
						   	bw.flush();
						}   
					}
				}
		   	}
			br.close();
			bw.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	
	//Data Change Function
	//ALL : ETM, JKB, JKC, JKG, JKO, JKQ, JKS, MAG, MAJ, NNB, NNG, NNP, VCN, VCP, XPN, XSA, XSN, XSV, EC, 
	//      EF, EP, IC, JC, JX, MM, NA, NP, NR, SE, SF, SL, SN, SO, SP, SS, SW, VA, VV, VX, XR, ETN......41
	//Invoked : (NNB, NNG, NNP, NR, XPN, XSN, SL, SW, XR), (VCP,VCN, VA, VV, VX, XSA), (SN), (MAG, MAJ, MM), (SS)......16
	//Not Invoked : JKQ, XSV, IC, NA ......4
	//Meaningless : SP(한국/NNP + ,/SP), NP(이/NP + 로써/JKB), ETM(오는	오/VV + 는/ETM), JKB(조치__04/NNG + 로/JKB), 
	//				JKC(개방형이	개방__04/NNG + 형/XSN + 이/JKC), JKG(주식__03/NNG + 의/JKG), JKO(마이너스/NNG + 를/JKO)
	//				JKS(자본금/NNG + 이/JKS), EC(보__01/VV + 아/EC), EF(같/VA + 다/EF + ./SF) EP(빚/VV + 었/EP + 던/ETM)
	//				JC(능력__02/NNG + 과/JC), JX(경제__04/NNG + 는/JX), SE(반등__01/NNG + …/SE), SF(있/VX + 다/EF + ./SF)
	//              SO(2/SN + ∼/SO + 3/SN + %/SW + 에/JKB), ETN(않/VX + 기/ETN) ......13
	String check(String str){
		String result = "";
		String type = "";
		String[] words = str.split(" \\+ ");
		for (String word : words ){
		    if(word.contains("NNB") || word.contains("NNP") || word.contains("NNG") || word.contains("XPN") || word.contains("XSN")
		    		|| word.contains("NR") || word.contains("SL") || word.contains("XR")){
		    	if(type.equals("/N ")) {
		    		if(result.contains("__")) {
		    			result = result.split("__")[0];
		    		}
		    		if(word.split("/")[0].contains("__")) {
		    			result = result +  word.split("/")[0].split("__")[0];
		    		}else {
		    			result = result +  word.split("/")[0];
		    		}
		    	}else {
		    		result = result +  word.split("/")[0];
		    	}
		    	type = "/N ";
		    }
		    else if(word.contains("VCP") || word.contains("VCN") || word.contains("XSA") || word.contains("VA") || word.contains("VV")
		    		|| word.contains("VX")){    
		    	result = result + word.split("/")[0] + "다";
		    	type = "/V ";
		    	break;						//'높/VA + 아/EC + 지/VX + 는/ETM'와 같은 복합 단어에서 대다수가 앞 동사가 주동사 그래서 break
		    }
		    else if(word.contains("SN")) {
		    	result = result + numChange(word.split("/")[0].length());
		    	type = "";
		    }
		    else if(word.contains("MAG") || word.contains("MAJ") || word.contains("MM")) {
		    	result = result +  word.split("/")[0];
		    	type = "/M ";
		    }
		    else if(word.contains("SS")) {
		    	if(result.contains("(") || result.contains(")")) {
	    			return "";
	    		}
		    	else{
		    		if(type.equals("/N ")) {
		    			result = result + type;
		    			type = "";
			    	}
		    	}
		    }
		    else if(word.contains("SW")) {
		    	if(!type.equals("")) {
	    		 	result = result +  word.split("/")[0];
			    	type = "/S ";
		    	}
		    }
		    else if(word.contains("SO")) {
	    		result = "";
			    type = "";
		    }
	    
		}
		if(result.equals("") || type.equals(""))
			return result;
		return result + type;
	}
	String numChange(int len) {
		String result;
		switch(len) {
    	case 1:
    		result = "N";
    		break;
    	case 2:
    		result = "NN";
    		break;
    	case 3:
    		result = "NNN";
    		break;
    	default:
    		result = "N+";
    	}
		return result;
	}
}