// This is Program used to show How to use Structure
#[derive(Debug)] // Add trait of debug, help us print value of structure fields
struct Rectangle {
    width: u32,
    height: u32,
}

fn main() {
    let rect = Rectangle { width : 30, height : 40};
    let acreage = area(&rect);
    println!("The area of rectangle {} is {:#?}", acreage, rect);
}

fn area (rectangles : &Rectangle) -> u32 {
    return rectangles.height * rectangles.width;
}
