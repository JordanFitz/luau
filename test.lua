img = Image:new("w:\\pictures\\download.png")

keys = {}

offset = {}
offset.x = 0
offset.y = 0

canvas.width(1280)
canvas.height(720)
canvas.use_vsync(true)
canvas.max_framerate(60)

canvas.load_font("Arial", "w:\\dev\\canvas\\arial.ttf")
context.font("30px Arial")

grad = context.create_linear_gradient(20,0, 220,0)

grad:add_color_stop(0, "green")
grad:add_color_stop(0.5, "cyan")
grad:add_color_stop(1, "green")

move_amt = 100

function canvas.update(delta)
    m = move_amt * delta

    if canvas.is_key_pressed("ArrowUp") then
        offset.y = offset.y - m
    end

    if canvas.is_key_pressed("ArrowDown") then
        offset.y = offset.y + m
    end

    if canvas.is_key_pressed("ArrowRight") then
        offset.x = offset.x + m
    end

    if canvas.is_key_pressed("ArrowLeft") then
        offset.x = offset.x - m
    end
end

function canvas.render()
    context.clear_rect()

    context.fill_style("red")
    context.stroke_style("red")
    context.line_width(10)
    context.stroke_rect(offset.x + 75, offset.y + 140, 150, 110)
    context.fill_rect(offset.x + 130,  offset.y + 190, 40, 60)

    context.begin_path()
    context.move_to(offset.x + 50, offset.y + 140)
    context.line_to(offset.x + 150, offset.y + 60)
    context.line_to(offset.x + 250, offset.y + 140)
    context.close_path()
    context.stroke()

    context.draw_image(img, 400, 400)

    context.fill_style("green")
    context.fill_text("Hello, world", 300, 100)

    context.line_width(1)
    context.stroke_rect(301, 90, context.measure_text("Hello, world").width, 50)

    context.fill_style(grad)
    context.fill_rect(offset.x+20, offset.y+20, 200, 100)
end
