function canvas.update()
    
end

function canvas.render()
    context.clear_rect()

    context.line_width(10)
    context.stroke_rect(75, 140, 150, 110)
    context.fill_rect(130, 190, 40, 60)

    context.begin_path()
    context.move_to(50, 140)
    context.line_to(150, 60)
    context.line_to(250, 140)
    context.close_path()
    context.stroke()
end

canvas.width(1280)
canvas.height(720)
