// Printf Fibonacci sequence
// Author: spacejiahao@outlook.com
// Date: 2020/8/8

fn fibonacci(n:u64) -> u64 {
    if n == 1 || n == 2 {
        return 1;
    } else {
        return fibonacci(n - 1) + fibonacci(n - 2);
    }
}
fn main() {
    println!("Hello, world!");
    let mut count: u64 = 0;
    loop {
        count += 1;
        if count > 30 {
            break;
        } else {
            print!("{:>2}: {:<20}", count, fibonacci(count));
        }

        if count % 3 == 0 {
            print!("\n");
        }
    }
}
