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
		File outFile = new File("output\\" + FileName + "_out.txt");
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
							System.out.println(Sentence);
							System.out.println(Sentence2);
							System.out.println();
//							bw.write(Sentence);
//						   	bw.newLine();
//						   	bw.write(Sentence2);
//						   	bw.newLine();
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
	//Invoked : (NNB, NNG, NNP, NR, XPN, XSN), (VCP,VCN, VA, VV, VX, XSA), (SN), (MAG, MAJ, MM)......16
	//Not Invoked : JKQ, XSV, IC, NA, SE, SF, SL, SO, SS, SW, XR, ETN......12
	//Meaningless : SP(�ѱ�/NNP + ,/SP), NP(��/NP + �ν�/JKB), ETM(����	��/VV + ��/ETM), JKB(��ġ__04/NNG + ��/JKB), 
	//				JKC(��������	����__04/NNG + ��/XSN + ��/JKC), JKG(�ֽ�__03/NNG + ��/JKG), JKO(���̳ʽ�/NNG + ��/JKO)
	//				JKS(�ں���/NNG + ��/JKS), EC(��__01/VV + ��/EC), EF(��/VA + ��/EF + ./SF) EP(��/VV + ��/EP + ��/ETM)
	//				JC(�ɷ�__02/NNG + ��/JC), JX(����__04/NNG + ��/JX)......13
	String check(String str){
		String result = "";
		String type = "";
		String[] words = str.split(" \\+ ");
		for (String word : words ){
		    if(word.contains("NNB") || word.contains("NNP") || word.contains("NNG") || word.contains("XPN") || word.contains("XSN")
		    		|| word.contains("NR")){    
		    	result = result +  word.split("/")[0];
		    	type = "/N ";
		    }
		    else if(word.contains("VCP") || word.contains("VCN") || word.contains("XSA") || word.contains("VA") || word.contains("VV")
		    		|| word.contains("VX")){    
		    	result = result + word.split("/")[0] + "��";
		    	type = "/V ";
		    	break;						//'��/VA + ��/EC + ��/VX + ��/ETM'�� ���� ���� �ܾ�� ��ټ��� �� ���簡 �ֵ��� �׷��� break
		    }
		    else if(word.contains("SN")) {
		    	result = result + numChange(word.split("/")[0].length());
		    	type = "";
		    }
		    else if(word.contains("MAG") || word.contains("MAJ") || word.contains("MM")) {
		    	result = result +  word.split("/")[0];
		    	type = "/M ";
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