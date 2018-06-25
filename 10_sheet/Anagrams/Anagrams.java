import java.io.IOException;
import java.util.HashSet;
import java.util.StringTokenizer;
import java.util.*;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class Anagrams {

  public static class TokenizerMapper
       extends Mapper<Object, Text, Text, Text>{

    private final static IntWritable one = new IntWritable(1);
    private Text hash = new Text();

    public void map(Object key, Text value, Context context
                    ) throws IOException, InterruptedException {
      StringTokenizer itr = new StringTokenizer(value.toString().toLowerCase());
      char[] hashChars;
      while (itr.hasMoreTokens()) {
        String word = itr.nextToken();
        hashChars = word.toCharArray();
        Arrays.sort(hashChars);
        hash.set(new String(hashChars));
        context.write(hash, new Text(word));
      }
    }
  }

  public static class AnagramReducer
       extends Reducer<Text,Text,Text,Text> {
    private Text result = new Text();

    public void reduce(Text key, Iterable<Text> values,
                       Context context
                       ) throws IOException, InterruptedException {
      HashSet<String> uniqueVals = new HashSet<>();
      int sum = 0;
      for (Text val : values) {
        for (String word : val.toString().split(" ")) {
          uniqueVals.add(word);
        }
      }
      StringBuilder resultBuilder = new StringBuilder();
      for (String word : uniqueVals) {
        resultBuilder.append(word).append(' ');
      }
      result.set(resultBuilder.toString());
      context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "Anagrams");
    job.setJarByClass(Anagrams.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setCombinerClass(AnagramReducer.class);
    job.setReducerClass(AnagramReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(Text.class);
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}
