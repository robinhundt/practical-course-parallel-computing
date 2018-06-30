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
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;

public class FriendFinder {

    public static class TokenizerMapper
            extends Mapper<Object, Text, Text, Text>{

        private Text newKey = new Text();
        private Text val = new Text();

        public void map(Object key, Text value, Context context
        ) throws IOException, InterruptedException {
            String[] friendIds = value.toString().trim().split("\t");
            newKey.set(friendIds[0]);
            val.set(friendIds[1]);
            context.write(newKey, val);
        }
    }

    public static class ValueConcat
            extends Reducer<Text,Text,Text,Text> {
        private Text result = new Text();

        public void reduce(Text key, Iterable<Text> values,
                           Context context
        ) throws IOException, InterruptedException {
            StringBuilder resultBuilder = new StringBuilder();
            for (Text val : values) {
                resultBuilder.append(val.toString()).append(";");
            }
            resultBuilder.deleteCharAt(resultBuilder.length()-1); //delete last ;
            result.set(resultBuilder.toString());
            context.write(key, result);
        }
    }

    public static class SortOfCrossProductMapper
            extends Mapper<Text, Text, Text, Text> {
        private Text newKey = new Text();

        public void map(Text key, Text value, Context context
        ) throws IOException, InterruptedException {
            String[] friendIds = value.toString().trim().split(";");
            for(String friendId : friendIds) {
                String[] sortedKeys = new String[]{key.toString(), friendId};
                Arrays.sort(sortedKeys);
                newKey.set("(" + sortedKeys[0] + ";" + sortedKeys[1] + ")");
                context.write(newKey, value);
            }
        }
    }

    public static class IntersectAndCountReducer
            extends Reducer<Text,Text,Text,IntWritable> {
        private IntWritable commonFriendsCount = new IntWritable();

        public void reduce(Text key, Iterable<Text> values,
                           Context context
        ) throws IOException, InterruptedException {
            ArrayList<String> friends = new ArrayList<String>();
            for(Text friendsText : values) {
                friends.add(friendsText.toString());
            }
            if(friends.size() >= 2) {
                HashSet<String> commonFriendSet = new HashSet<>(Arrays.asList(friends.get(0).split(";")));
                List<String> secondFriendList = Arrays.asList(friends.get(1).split(";"));
                commonFriendSet.retainAll(secondFriendList);
                commonFriendsCount.set(commonFriendSet.size());
                context.write(key, commonFriendsCount);
            }
        }
    }

    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        Path out = new Path(args[1]);


        Job job1 = Job.getInstance(conf, "FriendFinder");
        job1.setJarByClass(FriendFinder.class);
        job1.setMapperClass(TokenizerMapper.class);
        job1.setReducerClass(ValueConcat.class);
        job1.setOutputKeyClass(Text.class);
        job1.setOutputValueClass(Text.class);
        job1.setOutputFormatClass(SequenceFileOutputFormat.class);
        FileInputFormat.addInputPath(job1, new Path(args[0]));
        FileOutputFormat.setOutputPath(job1, new Path(out, "out1"));
        if (!job1.waitForCompletion(true)) {
            System.exit(1);
        }

        Job job2 = Job.getInstance(conf, "FriendFinder");
        job2.setJarByClass(FriendFinder.class);
        job2.setInputFormatClass(SequenceFileInputFormat.class);
        job2.setMapperClass(SortOfCrossProductMapper.class);
        job2.setMapOutputKeyClass(Text.class);
        job2.setMapOutputValueClass(Text.class);
        job2.setReducerClass(IntersectAndCountReducer.class);
        job2.setOutputKeyClass(Text.class);
        job2.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job2, new Path(out, "out1"));
        FileOutputFormat.setOutputPath(job2, new Path(out, "out2"));
        System.exit(job2.waitForCompletion(true) ? 0 : 1);
    }
}
