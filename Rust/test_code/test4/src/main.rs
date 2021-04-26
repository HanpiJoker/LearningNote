// Return a numbers list  average, median, mode
// Author: spacejiahao@outlook.com
// Date: 2020/8/8

use std::collections::HashMap; // for get mode

struct Numlist {
    v : Vec<u32>,
}

impl Numlist {
    fn new(v: Vec<u32>) -> Numlist {
        Numlist {
            v,
        }
    }
    fn average_num(&self) -> f32 {
        let mut tot_num : f32 = 0.0;
        for i in &self.v {
            tot_num += *i as f32;
        }
        return tot_num / self.v.len() as f32;
    }

    fn median_num(&self) -> f32 {
        // index start at zero
        if self.v.len() as u32 % 2 == 0 { // 偶数，取中间两数的平均值
            return (self.v[self.v.len() / 2 - 1] + self.v[self.v.len() / 2]) as f32;
        } else {   // 奇数，取中间
            return (self.v[self.v.len() / 2]) as f32;
        }
    }

    fn mode_num(&self) -> Vec<u32> {
        let mut mode_vec:Vec<u32> = Vec::new();
        let mut temp_map = HashMap::new();
        for num in &self.v {
            let count = temp_map.entry(num).or_insert(0);
            *count += 1;
        }

        for (key, value) in &temp_map {
            match temp_map.values().max() {
                Some(num) => {
                    if value == num {
                        mode_vec.push(**key);
                    }
                },
                _ => continue,
            };
        }

        return mode_vec;
    }
}
fn main() {
    println!("Number List Get Average Median and its mode");
    let mut input = String::new();
    let mut v:Vec<u32> = Vec::new();

    std::io::stdin().read_line(&mut input).expect("Read line failed");
    for value in input.split_whitespace() {
        match value.parse() {
            Ok(num) => v.push(num),
            Err(_) => continue,
        } 
    }
    v.sort();
    println!("Init number list is {:?}", v);

    let numlist = Numlist::new(v);
    println!("The average of number list is {}", numlist.average_num());
    println!("The median of number list is {}", numlist.median_num());
    println!("The mode of number list is {:?}", numlist.mode_num());
}
