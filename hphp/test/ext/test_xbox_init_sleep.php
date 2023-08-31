<?hh

<<__NEVER_INLINE>>
function do_usleep(int $useconds) {
    usleep($useconds);
}

function xbox_process_message(string $message) {
    // total of 10 seconds
    for ($i = 0; $i < 100; $i++) {
        do_usleep(100000); // 100ms
    }

    return strrev($message);
}
