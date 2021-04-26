// Fahrenheit and Celsius Temperature unit conversion
// Author: spacejiahao@outlook.com
// Date: 2020/8/8
use std::io;

fn get_input_unit() -> i32 {
    println!("Please Input what you want to do?");
    println!("1. F to C ( Fahrenheit to Celsius)");
    println!("2. C to F ( Celsius to Fahrenheit)");
    let mut value = String::new();
    io::stdin().read_line(&mut value)
        .expect("Failed to read line");
    let value : i32 = match value.trim().parse() {
        Ok(num) => num,
        Err(_) => -1,
    };

    return value;
}

fn get_input_temp() -> f32 {
    println!("Please input value you want to change");
    let mut temp = String::new();
    io::stdin().read_line(&mut temp)
        .expect("Failed to read line");

    let temp : f32 = match temp.trim().parse() {
        Ok(num) => num,
        Err(_) => 0.0,
    };

    return temp;
}

fn f2c_value(temp:f32) -> f32 {
    return ((temp - 32.0) * 5.0) / 9.0;
}

fn c2f_value(temp:f32) -> f32 {
    return (temp * 9.0) / 5.0  + 32.0;
}

fn main() {
    println!("Temperature C to F / F to C");
    loop {
        let value = get_input_unit();
        let temp = get_input_temp();
        if value == -1 || temp == 0.0 {
            continue;
        }

        let mut result: f32 = 0.0;
        let mut unit: char = '0';
        match value {
            1 => {
                result = f2c_value(temp);
                unit = 'C';
            },
            2 => {
                result = c2f_value(temp);
                unit = 'F';
            },
            _ => {
                println!("Unknown value!")
            },
        };
        println!("After change temperature is {}^{}", result, unit);
    }
}
