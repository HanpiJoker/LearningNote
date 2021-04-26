#[derive(Debug)]
enum Messages
{
    Quit,
    Move {x:i32, y:i32},
    Write(String),
    ChangeColor(i32, i32, i32),
}

#[derive(Debug)]
enum UsState {
    Alabama,
    Alaska,
    NewYork,
}

enum Coin {
    Penny,
    Nickey,
    Dime,
    Quarter(UsState),
}

fn value_in_cents(coin : Coin) -> u8 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickey => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("State quarter from {:?}!", state);
            25
        },
    }
}

fn value_in_cents_iflet(coin : Coin) -> u8 {
    let mut count = 0;
    if let Coin::Quarter(state) = coin {
        println!("State quarter from {:?}!", state);
        25
    } else {
        count += 1;
        count
    }
}

impl Messages {
    fn call (&self) {
        match &self {
            Messages::Quit => println!("call is Quit"),
            Messages::Write(string) => println!("Input string is {}", string),
            Messages::Move{x:_, y:_} => println!("Input two value is {:?}", self),
            Messages::ChangeColor(_x, _y, _z) => println!("Input tuple is {:?}", self),
        }
    }
}

fn plus_one(x: Option<i32>) -> Option<i32> {
    match x {
        None => None,
        Some(i) => Some(i + 1),
    }
}

fn main() {
    println!("Hello, world!");
    /* Match and Enum Use */
    let m = Messages::Write(String::from("Hello world"));
    m.call();
    let m = Messages::Quit;
    m.call();
    let m = Messages::Move{x : 1, y : 2};
    m.call();
    let m = Messages::ChangeColor(3, 2, 45);
    m.call();

    /* Option<> enum with none */
    let five = Some(5);
    let six = plus_one(five);
    println!("Input six {:#?}", six);
    let none = plus_one(None);
    println!("Input none {:#?}", none);

    /* _ Placeholder */
    let some_u8_value = 0u8;
    match some_u8_value {
        1 => println!("one"),
        _ => println!("It's not one, it's {}", some_u8_value),
    }   

    /* Enum Work with enum */
    let value = value_in_cents(Coin::Quarter(UsState::Alabama));
    println!("value is {}", value);

    let value2 = value_in_cents_iflet(Coin::Penny);
    println!("value is {}", value2);

}
