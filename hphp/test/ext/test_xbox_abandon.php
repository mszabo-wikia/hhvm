<?hh

enum CancelBehavior: int {
    NONE = 0;
    BEFORE_FINISH = 1;
    AFTER_FINISH = 2;
}

function print_boundary(string $message): void {
    $boundary_string = HH\Lib\Str\repeat('=', HH\Lib\Str\length($message));
    echo "\n{$boundary_string}\n";
    echo "{$message}\n";
    echo "{$boundary_string}\n\n";
}

function do_test(string $test_name, CancelBehavior $cancel_behavior, ?int $sleep_seconds = null): void {
    print_boundary("Starting test {$test_name}");

    $task = xbox_task_start('foo');
    echo "Started xbox task with input string: 'foo'\n";

    if ($sleep_seconds is nonnull) {
        sleep($sleep_seconds);
    }

    if ($cancel_behavior === CancelBehavior::BEFORE_FINISH) {
        echo "Cancelling xbox task before it finishes...\n";
        xbox_task_cancel($task);
    } else {
        echo "Letting xbox task complete!\n";
    }

    // Continue checking status until finished
    $finished = false;
    do {
        $finished = xbox_task_status($task);
        echo 'Task status: '.($finished ? 'finished' : 'not finished')."\n";
        sleep(1);
    } while (!$finished);

    // Check output of task
    $ret = null;
    $status_code = xbox_task_result($task, 1000, inout $ret);
    echo "Task ret: '{$ret}', Task status code: {$status_code}\n";

    if ($cancel_behavior === CancelBehavior::AFTER_FINISH) {
        try {
            echo "Cancelling xbox task after it finishes...\n";
            xbox_task_cancel($task);
        } catch (Exception $e) {
            echo 'Exception cancelling xbox task: '.$e->getMessage()."\n";
        }
    }

    print_boundary("Finished test {$test_name}");
}

<<__EntryPoint>>
function main() {
    do_test('let_task_run', CancelBehavior::NONE);
    do_test('cancel_with_sleep', CancelBehavior::BEFORE_FINISH, 1);
    do_test('cancel_after_done', CancelBehavior::AFTER_FINISH);
}