import cv2

def split_frame_into_boxes(image, num_boxes):
    height, width, _ = image.shape
    
    # Determine the number of boxes per side
    perimeter = 2 * (width + height)
    boxes_perimeter = perimeter / num_boxes

    # Calculate box sizes for each side
    boxes = []
    remaining_boxes = num_boxes
    
    # Top side
    top_boxes = int(min(remaining_boxes, width // boxes_perimeter))
    box_width_top = int(width / top_boxes) if top_boxes > 0 else width
    remaining_boxes -= top_boxes
    
    for i in range(top_boxes):
        x = int(i * box_width_top)
        y = 0
        boxes.append((x, y, box_width_top, boxes_perimeter))
    
    # Right side
    right_boxes = int(min(remaining_boxes, height // boxes_perimeter))
    box_height_right = int(height / right_boxes) if right_boxes > 0 else height
    remaining_boxes -= right_boxes
    
    for i in range(right_boxes):
        x = int(width - boxes_perimeter)
        y = int(i * box_height_right)
        boxes.append((x, y, boxes_perimeter, box_height_right))
    
    # Bottom side
    bottom_boxes = int(min(remaining_boxes, width // boxes_perimeter))
    box_width_bottom = int(width / bottom_boxes) if bottom_boxes > 0 else width
    remaining_boxes -= bottom_boxes
    
    for i in range(bottom_boxes):
        x = int(width - (i + 1) * box_width_bottom)
        y = int(height - boxes_perimeter)
        boxes.append((x, y, box_width_bottom, boxes_perimeter))
    
    # Left side
    left_boxes = int(min(remaining_boxes, height // boxes_perimeter))
    box_height_left = int(height / left_boxes) if left_boxes > 0 else height
    remaining_boxes -= left_boxes
    
    for i in range(left_boxes):
        x = 0
        y = int(height - (i + 1) * box_height_left)
        boxes.append((x, y, boxes_perimeter, box_height_left))

    # Draw boxes for visualization
    for x, y, w, h in boxes:
        cv2.rectangle(image, (x, y), (int(x + w), int(y + h)), (0, 255, 0), 2)

    return image, boxes