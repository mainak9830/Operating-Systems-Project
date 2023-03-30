import java.util.ArrayList;
import java.util.LinkedList;
import java.io.File; // Import the File class
import java.io.FileNotFoundException; // Import this class to handle errors
import java.util.Scanner;



import java.util.HashMap;

class MapReduce {
    ArrayList<Thread> reduceThreads;
    ArrayList<LinkedList<String>> list;
    ArrayList<String> output;
    ArrayList<Integer> complete, reducerCompleted;
    ArrayList<Integer> active;
    HashMap<String, HashMap<String, ArrayList<Integer>>> wordCount;
    int LIMIT = 10;
    int reducers;
    
    
    class Mapper implements Runnable {
        int recordId;
        String file;

        Mapper(int recordId, String file) {
            this.recordId = recordId;
            this.file = file;
        }

        public void run() {
            
            try {
                File myObj = new File(file);
                Scanner myReader = new Scanner(myObj);
                myReader.useDelimiter("\n");
                int line = 1;
                while (myReader.hasNextLine()) {
                    String data = myReader.nextLine();
                    for (String word : data.split(" ")) {
                        word = word.replaceAll("[^A-Za-z0-9 ]", " ");
                        word = word.toLowerCase();
                        int nReducer = (word.hashCode() % reducers + reducers) % reducers;
                        try {
                            LinkedList<String> mlist = list.get(nReducer);
                            synchronized (mlist) {
                                while (mlist.size() == LIMIT) {
                                    //System.out.println("Buffer Full" + "cannot produce !!!");
                                    mlist.wait();
                                }
                                // buffer[nReducer][full[nReducer]] = word + " # " + nReducer + " # " + line + "
                                // # " + file;
                                // System.out.println(buffer[nReducer][full[nReducer]]);
                                // full[nReducer] += 1;
                                //System.out.println("Producer - " + recordId + " " + word + "\n");
                                mlist.add(word + "#" + file + "#" + line);
                                mlist.notifyAll();
                            }
                        } catch (Exception e) {

                        }

                    }

                    line += 1;
                }
                myReader.close();
            } catch (FileNotFoundException e) {
                //System.out.println("An error occurred.");
                e.printStackTrace();
            }
                
            
            synchronized(active){
                active.add(0, active.get(0)-1);

                if(active.get(0) == 0){
                    

                    try {
                        for(int i = 0;i < reducers;i++){
                            LinkedList<String> rlist = list.get(i);
                            while(rlist.size() > 0){
                                active.wait();
                            }
                            
                            //rlist.notifyAll();
                        }
                        // synchronized(reducerCompleted){
                        //     System.out.println(active + "active !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                        //     while(reducerCompleted.get(0) > 0){
                        //         for(int i = 0;i < reducers;i++){
                        //             LinkedList<String> rlist = list.get(i);
                        //             synchronized(rlist){
                        //                 rlist.add("endToken");
                        //                 rlist.notifyAll();
                        //             }
                                    
                        //         }
                        //         reducerCompleted.wait();
                        //     }
                        // }
                        // activeObj.wait();
                        active.notifyAll();
                        //System.out.println("waited for all buffer to be cleared !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    
                        synchronized(complete){
                            complete.set(0,1);
                            complete.notifyAll();
                            complete.wait();
                        }
                    } catch (Exception e) {
                        // TODO: handle exception
                    }
                    
                }
            }
            //     synchronized((Object)active){
            //         active--;
            //         if(active == 0){
            //             System.out.println(active);
            //         }
            //     }
                
            // } catch (Exception e) {
            //     // TODO: handle exception
            // }
            
        }
    }

    class Reducer implements Runnable {
        int recordId;
        boolean exit;
        Reducer(int recordId) {
            this.recordId = recordId;
            exit = false;
        }

        public void run() {
            while (!exit) {
                
                LinkedList<String> rlist = list.get(recordId);
                synchronized (rlist) {
                    // consumer thread waits while list
                    // is empty
                    
                    while (rlist.size() == 0) {
                        try {
                            //space Try new
                            
                            
                            
                            //System.out.println("Going to sleep" + recordId);
                            
                                //space Try new
                            rlist.wait();
                            
                            synchronized(active){
                                if(active.get(0) == 0){
                                    //System.out.println("maptask completed !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" + recordId);
                                    synchronized(reducerCompleted){
                                        reducerCompleted.add(0, reducerCompleted.get(0)-1);
                                        exit = true;
                                        reducerCompleted.notifyAll();
                                        
                                    }
                                }
                                // while(active.get(0) == 0 && rlist.size() > 0){
                                //     exit = true;
                                    
                                //     continue;
                                // }

                                
                            }
                            if(exit)
                                break;
                            

                            
                        } catch (Exception e) {
                            // TODO: handle exception
                        }
                    }
                    
                    if(exit)
                        break;
                    // to retrieve the first job in the list
                    String val = rlist.removeFirst();
                    
                    // System.out.println("Consumer consumed- "
                    //         + recordId + " " + val + "\n");
                    synchronized(output){
                        
                        output.add(val);
                        
                        
                        
                    }
                    // Wake up producer thread
                    rlist.notify();
                    
                    synchronized(active){
                        active.notify();
                    }
                    
                    // and sleep
                    // Thread.sleep(1000);
                }
            }
            //System.out.println(" I am free !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" + recordId);
            
        }

        public void stopexec(){
            exit = true;
        }
        

        
    }

    public void clean(){
        for(String res : output){
            
            String arr[] = res.split("#");
            if(!wordCount.containsKey(arr[0])){
                HashMap<String, ArrayList<Integer>> tlist = new HashMap<String, ArrayList<Integer>>();
                ArrayList<Integer> indices = new ArrayList<>();
                indices.add(Integer.parseInt(arr[2]));
                tlist.put(arr[1], indices);
                wordCount.put(arr[0], tlist);
            }
            else{
                HashMap<String, ArrayList<Integer>> tlist = wordCount.get(arr[0]);
                if(!tlist.containsKey(arr[1])){
                    ArrayList<Integer> indices = new ArrayList<>();
                    indices.add(Integer.parseInt(arr[2]));
                    tlist.put(arr[1], indices);
                    wordCount.put(arr[0], tlist);

                }else{
                    
                    tlist.get(arr[1]).add(Integer.parseInt(arr[2]));
                    wordCount.put(arr[0], tlist);
                }
                
                
            }

            }

            for (String key : wordCount.keySet()) {
                HashMap<String, ArrayList<Integer>> hmap = wordCount.get(key);
                System.out.print(key + " ");
                for(String fileName : hmap.keySet()){
                    System.out.print(fileName + "  @  ");

                    for(Integer i : hmap.get(fileName)){
                        System.out.print(i + ", ");
                    }
                    System.out.print("    ");
                }
                System.out.println("");
                 
            }
        
        
            
       
    }

    public void controller(String[] args) {
        active = new ArrayList<>();
        reducerCompleted = new ArrayList<>();
        active.add(args.length - 1);
        reducers = Integer.parseInt(args[0]);
        reducerCompleted.add(reducers);
        output = new ArrayList<String>();
        list = new ArrayList<LinkedList<String>>();
        complete = new ArrayList<>();
        wordCount = new HashMap<String, HashMap<String, ArrayList<Integer>>>();
        complete.add(0);
        for(int i = 0;i < reducers;i++){
            list.add(new LinkedList<String>());
        }
        
        for (int i = 1; i <= args.length - 1; i++) {
            Thread mthread = new Thread(new Mapper(i - 1, args[i]));
            mthread.start();
        }

        for (int i = 0; i < reducers; i++) {
            Thread rthread = new Thread(new Reducer(i));

            try {
                reduceThreads.add(rthread);
            } catch (Exception e) {
                // TODO: handle exception
            }
            
            rthread.start();
        }
        

       
        synchronized(complete){
            while(complete.get(0) == 0){
                try {
                    complete.wait();

                } catch (Exception e) {
                    // TODO: handle exception
                }
                
            }
            //System.out.println("completed !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
           // System.out.println("size word" + wordCount.size());
           
            

        }
        //
        synchronized(output){
            clean();
        }
        // try {
        //     sem.acquire();
            
        //     while(active > 0)
        //         this.wait();
            
            

        //     sem.release();
        // } catch (Exception e) {
        //     // TODO: handle exception
        // }
        

    }

    public static void main(String[] args) {
        
        MapReduce mr = new MapReduce();
        mr.controller(args);
        return;
    }
}
