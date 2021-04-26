fn main() {
    // String Variable statement
    let s1 = String::from("hello");

    // Variables and Data Interact: 1. Move
    let s2 = s1;    // s1 is invalid after this statements;

    println!("s2 is {}", s2);

    // Variables and Data Interact: 2. Clone
    let s3 = s2.clone();
    println!("after clone s2 is {}", s2);
    println!("after clone s3 is {}", s3);

    take_ownership(s3);     // After this statements, s3 is invalid

    // Variables and Data Interact: 3. Copy Only for Stack
    let u1 = 32;
    let u2 = u1;
    println!("after copy u1 is {}", u1);
    stack_copy(u2);     // After this statments, u2 is still valid.
    println!("after copy u2 is {}", u2);

    let s4 = given_ownership();
    println!("after given ownership call s4 is {}", s4);

    let s5 = take_and_given_ownership(s4);
    println!("after take and given ownership call s5 is {}", s5);
}

fn take_ownership(some_string: String) {
    println!("println! in function {}", some_string);
}

fn stack_copy(some_integer: u32) {
    println!("println! in function {}", some_integer);
}

fn given_ownership() -> String {
    let some_string = String::from("hello in function");
    return some_string; // Only write some_string is also supported;
}

fn take_and_given_ownership(some_string: String) -> String {
    let b_string = some_string;
    b_string
}