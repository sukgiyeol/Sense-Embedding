import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.util.ArrayList;

public class format_01 {
	BufferedWriter bw = null;
	BufferedReader br = null;
	format_01(String inputFile){
		String FileName = inputFile.split("\\.")[0];
		File inFile = new File("input\\" + FileName);
		File outFile = new File("output\\text99.txt");
		if(!outFile.exists()) {
			try {
				outFile.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	
		try {
			String line;
			br = new BufferedReader(new InputStreamReader(new FileInputStream(inFile), "MS949")); 
			bw = new BufferedWriter(new FileWriter(outFile, true));
			String str;
			byte[] theByteArray;
			while ((line = br.readLine()) != null) {
				theByteArray = line.getBytes("UTF-8");
			    str = new String(theByteArray, Charset.forName("UTF-8"));
			    check(str);
			}
			br.close();
			bw.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
	}
	void check(String str){
		String result = "";
		String[] words = str.split("	");
		
		for (String word : words ){
			try {
				bw.write(word.split(" ")[1].split("/")[0]);
				System.out.print(word.split(" ")[1].split("/")[0]);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		try {
			System.out.println();
			bw.newLine();
			bw.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}