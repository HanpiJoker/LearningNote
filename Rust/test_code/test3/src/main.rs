// Print Lyric of The twelve days of christmas
// Author: spacejiahao@outlook.com
// Date: 2020/8/8

fn main() {
    let times = ["first", "second", "third",
                 "forth", "fifth", "sixth",
                 "seventh", "eighth", "ninth",
                 "tenth", "eleventh", "twelfth"];
    let gifts = [ "A partridge in a pear tree",
                  "Two turtle doves",
                  "Three French hens",
                  "Four calling birds",
                  "Five golden rings",
                  "Six geese a-laying",
                  "Seven swans a-swimming",
                  "Eight maids a-milking",
                  "Nine ladies dancing",
                  "Ten lords a-leaping",
                  "Eleven pipers piping",
                  "Twelve drummers drumming"];
    println!("<<The twelve days of christmas>>");
    let mut index = 0;
    loop {
        if index >= 12 {
            break;
        }
        println!("On the {} day of Christmas, my true love sent to me: ", times[index]);
        let mut sec_index = index;
        loop {
            print!("\t");
            if sec_index == 0 && index != 0 {
                println!("And {}.", gifts[sec_index]);
                break;
            } else if index == 0 {
                println!("{}.", gifts[sec_index]);
                break;
            }
            println!(" {},", gifts[sec_index]);
            sec_index = sec_index - 1;
        }
        index += 1;
    }
}
