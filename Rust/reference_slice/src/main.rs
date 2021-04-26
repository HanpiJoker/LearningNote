fn main() {
    let s1 = String::from("Hello World");
    println!("Length is {} of {}", caclulate_length(&s1), s1);

    let mut s2 = s1;
    change_string(&mut s2);
    println!("after change is {}, it's size is {}", s2, caclulate_length(&s2));

    // We Cannot use mutable reference and immutable reference at same time.
    let r1 = &s2;
    let r2 = &s2;
    println!("Double immutable reference of a mutable variable is {} {}", r1, r2);

    // We Don't use r1、r2 at next time, so we can create a mutable reference at now
    let r3 = &mut s2;
    println!("creat mutable reference of a mutable variable is {}", r3);
    // IMPORTANT: In Rust, automatic referencing and dereferencing is supported. So 
    // we can println r1/r2/r3 directly output a string.

    let word = first_word(&s2);
    println!("after first_word function called {}", word);
    let word_len = first_word_without_slice(&s2);
    println!("after first_word function called {}", word_len);
}

// immutable reference
fn caclulate_length(input_string: &String) -> usize {
    return input_string.len();
}

// mutable reference
fn change_string(input_string: &mut String) {
    input_string.push_str(", Yujiahao");
}

// Slice Use Function
fn first_word_without_slice(s: &String) -> usize {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return i;
        }
    }

    return s.len();
}

fn first_word(s: &String) -> &str {
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];
        }
    }

    return &s[..];
}