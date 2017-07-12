import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;

public class main {
   public static void main(String args[]) throws IOException{
	   File dirFile=new File("input");
	   File []fileList=dirFile.listFiles();
	   format_01 format01;
	   format_02 format02;
	   for(File tempFile : fileList) {
		   if(tempFile.isFile()) {
			   String tempFileName=tempFile.getName();
//             System.out.println("FileName="+tempFileName);
			   if(tempFileName.contains("txt")) {	   
				   format01 = new format_01(tempFileName);
				   format02 = new format_02(tempFileName);
			   }
		   }
	   }
//      format01 = new format_01("BSAA0002.txt");
   	}
}