// This is Program used to show How to use method
#[derive(Debug)] // Add trait of debug, help us print value of structure fields
struct Rectangle {
    width: u32,
    height: u32,
}

impl Rectangle {
    fn area (&self) -> u32 {
        return self.height * self.width;
    }

    fn can_hold (&self, other: &Rectangle) -> bool {
        return (self.width > other.width) && (self.height > other.height);
    }
}

impl Rectangle {
    fn square (size : u32) -> Rectangle {
        return Rectangle{width:size, height:size};
    }
}

fn main() {
    let rect  = Rectangle { width : 30, height : 40};
    let rect1 = Rectangle { width : 20, height : 30};
    let rect2 = Rectangle { width : 50, height : 20};
    let acreage = rect.area();
    println!("The area of rectangle {} is {:#?}", acreage, rect);
    println!("Can rect {:#?} hold rect1 {:#?} ? {}", rect, rect1, rect.can_hold(&rect1));
    println!("Can rect {:#?} hold rect2 {:#?} ? {}", rect, rect2, rect.can_hold(&rect2));
    let sq = Rectangle::square(32);
    let area_sq = sq.area();
    println!("The area of rectangle {} is {:#?}", area_sq, sq);
}
