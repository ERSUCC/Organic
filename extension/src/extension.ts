import { commands, type ExtensionContext, Uri, window } from "vscode";
import { ChildProcess, spawn } from "node:child_process";

interface Process {
  path: string;
  proc: ChildProcess;
  killed: boolean;
}

let procs = new Map<string, Process>();

export function activate(context: ExtensionContext) {
  context.subscriptions.push(commands.registerCommand("organic.run", (uri: Uri) => {
    const path: string = uri.fsPath;
    const current: Process | undefined = procs.get(path);

    if (current) {
      procs.delete(path);

      current.killed = true;
      current.proc.kill("SIGINT");
    }

    const name: string | undefined = path.split(/[/\\]/).pop();

    window.showInformationMessage(`Running ${name}`);

    const proc: Process = {
      path: path,
      proc: spawn("organic", [ path ]),
      killed: false
    };

    procs.set(path, proc);

    proc.proc.on("error", () => {
      window.showErrorMessage(`Failed to run ${name}`);

      procs.delete(path);
    }).on("exit", (code: number) => {
      if (code != 0 && !proc.killed) {
        window.showErrorMessage(`Failed to run ${name}`);

        procs.delete(path);
      }
    });
  }));

  context.subscriptions.push(commands.registerCommand("organic.stop", (uri: Uri) => {
    const path: string = uri.fsPath;
    const current: Process | undefined = procs.get(path);

    if (current) {
      window.showInformationMessage(`Stopping ${path.split(/[/\\]/).pop()}`);

      procs.delete(path);

      current.killed = true;
      current.proc.kill("SIGINT");
    }
  }));
}
