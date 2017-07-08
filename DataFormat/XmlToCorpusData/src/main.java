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
      for(File tempFile : fileList) {
        if(tempFile.isFile()) {
          String tempFileName=tempFile.getName();
          //System.out.println("FileName="+tempFileName);
          //format_01 format01 = new format_01(tempFileName);
        }
      }
      format_01 format01 = new format_01("BSAA0002.txt");
   }
}