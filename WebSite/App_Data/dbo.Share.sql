CREATE TABLE [dbo].[Share] (
    [Id]        INT           NOT NULL,
    [country]   VARCHAR (50)  NULL,
    [company]   VARCHAR (50)  NULL,
    [isin_code] VARCHAR (50)  NULL,
    [ticker]    VARCHAR (50)  NULL,
    [path]      VARCHAR (100) NULL,
    PRIMARY KEY CLUSTERED ([Id] ASC),
    CONSTRAINT [FK_Share_ToFinancial_Product] FOREIGN KEY ([Id]) REFERENCES [dbo].[Financial_Product] ([Id])
);