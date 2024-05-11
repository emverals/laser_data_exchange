import curses
import serial
import curses.textpad
import threading


def thr_in_w(port, input_win, out_win, prog_win, width):
    global COUNT_LINES
    while True:
        # input_win.addstr(input_win.getmaxyx()[0] // 2, 2, str(random.randint(1, 10)))
        # input_win.refresh()
        curses.echo()
        input_win.addstr(input_win.getmaxyx()[0] // 2, 2, 'Отправить: ')
        input_win.refresh()
        user_input = input_win.getstr().decode('utf-8')
        if user_input:
            COUNT_LINES += 1
            try:
                port.write(user_input.encode('utf-8'))
                out_win.addstr(COUNT_LINES, 2, "me > " + user_input)

            except Exception:
                pass
            input_win.clear()
            out_win.refresh()
            input_win.border()


def thr_main_w(stdscr):
    while True:
        stdscr.refresh()


def thr_out_w(port, out_win):
    global COUNT_LINES
    while True:
        # out_win.addstr(out_win.getmaxyx()[0] // 2, 2, str(random.randint(1, 10)))
        # out_win.refresh()
        received_message = port.readline().strip().decode()
        if received_message:
            COUNT_LINES += 1
            out_win.addstr(COUNT_LINES, 2, "friend > " + received_message)
            out_win.refresh()
            out_win.refresh()  # Обновляем окно


COUNT_LINES = 0


def main(stdscr):
    port = serial.Serial('COM3', 9600)

    curses.curs_set(0)
    stdscr.clear()
    height, width = stdscr.getmaxyx()

    input_win = curses.newwin(height // 4, width // 10 * 7, 0, 0)
    out_win = curses.newwin(height - height // 4, width // 10 * 7, height // 4, 0)
    prog_win = curses.newwin(height, width // 10 * 3, 0, width - width // 10 * 3)

    prog_win.border()
    input_win.border()
    out_win.border()

    stdscr.refresh()
    input_win.refresh()
    out_win.refresh()
    prog_win.refresh()

    thr_in_w_p = threading.Thread(target=thr_in_w, args=(port, input_win, out_win, prog_win, width))
    thr_main_w_p = threading.Thread(target=thr_main_w, args=(stdscr,))
    thr_out_w_p = threading.Thread(target=thr_out_w, args=(port, out_win,))

    thr_out_w_p.start()
    thr_in_w_p.start()
    thr_main_w_p.start()


curses.wrapper(main)
